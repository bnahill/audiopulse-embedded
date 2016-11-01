/*!
 (C) Copyright 2015 Ben Nahill

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.

 @file uart.cpp
 @brief
 @author Ben Nahill <bnahill@gmail.com>
 */

#include <driver/uart.h>

bool UART::init(uint32_t br_target){

    __disable_irq();
    if(is_init){
        __enable_irq();
        return true;
    }

    rx.configure(GPIOPin::MUX_ALT3);
    tx.configure(GPIOPin::MUX_ALT3);

    // The value to be used in generating the module clock
    float divider = Clock::config.mcgoutclk * 32 / (br_target * 16);
    uint16_t br_reg = divider / 32;
    uint16_t fine_adjust = ((uint32_t)divider) % 32;

    dev.BDH = UART_BDH_SBR(br_reg >> 5);
    dev.BDL = UART_BDL_SBR(br_reg);


    dev.C1 =
        0;
    dev.C4 = UART_C4_BRFA(fine_adjust);
    dev.MA1 = 0;
    dev.MA2 = 0;

    dev.C2 =
        UART_C2_TIE_MASK |
        UART_C2_TE_MASK;
    dev.C3 = 0;
    dev.S2 = 0;
    dev.C5 =
        UART_C5_TDMAS_MASK;

    DMA_CSR_REG(DMA0, dma_ch_tx) = 0;
    // First configure receive channel
    DMAMUX_CHCFG(dma_ch_tx) = 0;
    NVIC_EnableIRQ((IRQn_Type)dma_ch_tx);

    DMA_DADDR_REG(DMA0, dma_ch_tx) = (uint32_t)&dev.D; // Transmit FIFO
    DMA_SADDR_REG(DMA0, dma_ch_tx) = (uint32_t)0;
    DMA_SOFF_REG(DMA0, dma_ch_tx) = 1; // Increment source!
    DMA_DOFF_REG(DMA0, dma_ch_tx) = 0; // Don't increment destination!
    DMA_ATTR_REG(DMA0, dma_ch_tx) =
        DMA_ATTR_SMOD(0) | DMA_ATTR_SSIZE(0) | // 8-bit src
        DMA_ATTR_DMOD(0) | DMA_ATTR_DSIZE(0);  // 8-bit dst
    DMA_NBYTES_MLNO_REG(DMA0, dma_ch_tx) = 1;
    DMA_NBYTES_MLOFFNO_REG(DMA0, dma_ch_tx) = 1;
    DMA_NBYTES_MLOFFYES_REG(DMA0, dma_ch_tx) = 1;
    // CITER and BITER must be the same
    DMA_CITER_ELINKYES_REG(DMA0, dma_ch_tx) = 0;
    DMA_BITER_ELINKYES_REG(DMA0, dma_ch_tx) = 0;
    DMA_CITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_CITER_ELINKNO_CITER(1);
    DMA_BITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_BITER_ELINKNO_BITER(1);
    DMA_SLAST_REG(DMA0, dma_ch_tx) = 0;

    DMA_DLAST_SGA_REG(DMA0, dma_ch_tx) = 0;

    DMA_CSR_REG(DMA0, dma_ch_tx) = DMA_CSR_INTMAJOR_MASK | // Major loop IRQ
                DMA_CSR_DREQ_MASK |
                DMA_CSR_BWC(0); // 0-cycle delay

    DMA_SERQ = DMA_SERQ_SERQ(dma_ch_tx); // Enable requests

    if(&dev == UART0_BASE_PTR){
        DMAMUX_CHCFG_REG(DMAMUX, dma_ch_tx) = DMAMUX_CHCFG_SOURCE(3) |
                                              DMAMUX_CHCFG_ENBL_MASK;
        //DMAMUX_CHCFG_REG(DMAMUX, dma_ch_rx) = DMAMUX_CHCFG_SOURCE(2) |
        //                                      DMAMUX_CHCFG_ENBL_MASK;
    } else if(&dev == UART1_BASE_PTR){
        DMAMUX_CHCFG_REG(DMAMUX, dma_ch_tx) = DMAMUX_CHCFG_SOURCE(5) |
                                              DMAMUX_CHCFG_ENBL_MASK;
        //DMAMUX_CHCFG_REG(DMAMUX, dma_ch_rx) = DMAMUX_CHCFG_SOURCE(4) |
        //                                      DMAMUX_CHCFG_ENBL_MASK;
    } else {
        while(true);
    }

    // TODO: Verify that this line is necessary    
    DMA_CSR_REG(DMA0, dma_ch_tx) |= DMA_CSR_ACTIVE_MASK; // Set actuve

    is_init = true;
    __enable_irq();

    return true;
}

bool UART::enqueue(uint8_t const * src, uint16_t len, cb_t cb, void * cb_arg){
    if(len == 0){
        if(cb)
            cb(cb_arg);
        return true;
    }

    __disable_irq();
    if(queue_count >= queue_len){
        __enable_irq();
        return false;
    }
    queue_count += 1;
    queue_data[queue_wr_head].src = src;
    queue_data[queue_wr_head].len = len;
    queue_data[queue_wr_head].cb = cb;
    queue_data[queue_wr_head].cb_arg = cb_arg;
    queue_wr_head = (queue_wr_head + 1) % queue_len;
    if(queue_count == 1){
        do_tx(src, len);
    }
    __enable_irq();
    return true;
}

bool UART::do_tx(uint8_t const * src, uint16_t len){
    if(tx_busy)
        return false;
    tx_busy = true;

    DMA_CITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_CITER_ELINKNO_CITER(len);
    DMA_BITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_BITER_ELINKNO_BITER(len);

    DMA_SADDR_REG(DMA0, dma_ch_tx) = (uint32_t)src;
    DMA_SOFF_REG(DMA0, dma_ch_tx) = 1; // Increment source!
    // Start transmit
    DMA_SSRT = DMA_SSRT_SSRT(dma_ch_tx);

    return true;
}

void UART::dequeue(){
    cb_t cb;
    void * cb_arg;
    __disable_irq();
    if(queue_count == 0){
        __enable_irq();
        return;
    }

    tx_busy = 0;
    queue_count -= 1;
    cb_arg = queue_data[queue_rd_head].cb_arg;
    cb = queue_data[queue_rd_head].cb;
    queue_rd_head = (queue_rd_head + 1) % queue_len;


    if(queue_count)
        do_tx(queue_data[queue_rd_head].src, queue_data[queue_rd_head].len);

    __enable_irq();
}
