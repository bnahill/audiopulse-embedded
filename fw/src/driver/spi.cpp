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

 @file spi.cpp
 @brief
 @author Ben Nahill <bnahill@gmail.com>
 */

#include "driver/spi.h"


void SPI::register_slave(SPI_slave &slave){
	if(slave.index != ~0)
		return;

	if(nslaves > 1)
		return;

    spi->MCR |= SPI_MCR_MDIS_MASK;

	slave.index = nslaves;
    slave.NCS.configure((GPIOPin::mux_t)slave.mux, true);
    //slave.NCS.configure(GPIOPin::MUX_GPIO);
    //slave.NCS.make_output();
    //slave.NCS.set();
	spi->CTAR[nslaves] = slave.CTAR;
	nslaves += 1;

    spi->MCR &= ~SPI_MCR_MDIS_MASK;
}

void SPI::dma_init(){
    // General DMA setup

    DMAMUX_CHCFG_REG(DMAMUX, dma_ch_tx) = 0;
    DMAMUX_CHCFG_REG(DMAMUX, dma_ch_rx) = 0;

    spi->RSER = SPI_RSER_RFDF_RE_MASK | SPI_RSER_RFDF_DIRS_MASK |
                SPI_RSER_TFFF_RE_MASK | SPI_RSER_TFFF_DIRS_MASK;

    //
    // Set up RX channel
    //

    DMA_DADDR_REG(DMA0, dma_ch_rx) = (uint32_t)0; // Transmit FIFO
    DMA_SADDR_REG(DMA0, dma_ch_rx) = (uint32_t)&spi->POPR;
    DMA_SOFF_REG(DMA0, dma_ch_rx) = 1; // Increment source!
    DMA_DOFF_REG(DMA0, dma_ch_rx) = 0; // Don't increment destination!
    DMA_ATTR_REG(DMA0, dma_ch_rx) = DMA_ATTR_SMOD(0) | DMA_ATTR_SSIZE(0) | // 8-bit src
                                    DMA_ATTR_DMOD(0) | DMA_ATTR_DSIZE(0);  // 8-bit dst
    DMA_NBYTES_MLNO_REG(DMA0, dma_ch_rx) = 1;
    DMA_NBYTES_MLOFFNO_REG(DMA0, dma_ch_rx) = 1;
    DMA_NBYTES_MLOFFYES_REG(DMA0, dma_ch_rx) = 1;
    // CITER and BITER must be the same
    DMA_CITER_ELINKYES_REG(DMA0, dma_ch_rx) = 0;
    DMA_BITER_ELINKYES_REG(DMA0, dma_ch_rx) = 0;
    DMA_CITER_ELINKNO_REG(DMA0, dma_ch_rx) = DMA_CITER_ELINKNO_CITER(1);
    DMA_BITER_ELINKNO_REG(DMA0, dma_ch_rx) = DMA_BITER_ELINKNO_BITER(1);
    DMA_SLAST_REG(DMA0, dma_ch_rx) = 0;

    DMA_DLAST_SGA_REG(DMA0, dma_ch_rx) = 0;

    DMA_CSR_REG(DMA0, dma_ch_rx) = DMA_CSR_INTMAJOR_MASK | // Major loop IRQ
                DMA_CSR_DREQ_MASK |
                DMA_CSR_BWC(0); // 0-cycle delay

    DMA_SERQ = DMA_SERQ_SERQ(dma_ch_rx); // Enable ch 0 requests

    //
    // Set up TX channel
    //

    DMA_DADDR_REG(DMA0, dma_ch_tx) = (uint32_t)&spi->PUSHR; // Transmit FIFO
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

    // No interrupt from TX channel
    DMA_CSR_REG(DMA0, dma_ch_tx) = DMA_CSR_BWC(0) |
                DMA_CSR_DREQ_MASK;

    DMA_SERQ = DMA_SERQ_SERQ(dma_ch_tx); // Enable ch 0 requests


    //
    // Connect the DMA sources
    //

    if(spi == SPI0_BASE_PTR){
        DMAMUX_CHCFG_REG(DMAMUX, dma_ch_tx) = DMAMUX_CHCFG_SOURCE(15) |
                                              DMAMUX_CHCFG_ENBL_MASK;
        DMAMUX_CHCFG_REG(DMAMUX, dma_ch_rx) = DMAMUX_CHCFG_SOURCE(14) |
                                              DMAMUX_CHCFG_ENBL_MASK;
    } else if(spi == SPI1_BASE_PTR){
        DMAMUX_CHCFG_REG(DMAMUX, dma_ch_tx) = DMAMUX_CHCFG_SOURCE(16) |
                                              DMAMUX_CHCFG_ENBL_MASK;
        DMAMUX_CHCFG_REG(DMAMUX, dma_ch_rx) = DMAMUX_CHCFG_SOURCE(16) |
                                              DMAMUX_CHCFG_ENBL_MASK;
    }
}

bool SPI::transfer(SPI_slave &slave,
                   uint8_t const * tx_buf, uint8_t * rx_buf,
                   uint32_t count,
                   spi_cb_t cb, void * arg){
    static uint8_t zero = 0;
    static uint8_t dummy_dest = 0;

    // Check to make sure slave is assigned
    if(slave.index == ~0)
        return false;

    if((not rx_buf) and (not tx_buf))
        return false;

    if(busy)
        return false;

    if(count == 0)
        return true;

    busy = true;

    current_cb = cb;
    current_arg = arg;
    current_slave = &slave;


    spi->MCR = SPI_MCR_HALT_MASK |
            SPI_MCR_MSTR_MASK |    // Master
            SPI_MCR_DCONF(0) |     // SPI mode
            SPI_MCR_PCSIS(1) |     // All nCS idle high
            SPI_MCR_DIS_RXF_MASK | // No RX FIFO
            SPI_MCR_DIS_TXF_MASK | // No TX FIFO
            SPI_MCR_CLR_TXF_MASK |
            SPI_MCR_CLR_RXF_MASK |
            SPI_MCR_SMPL_PT(0);    // Sample on clock edge

    spi->PUSHR =
            SPI_PUSHR_CTAS(slave.index) |
            SPI_PUSHR_CONT_MASK |
            SPI_PUSHR_CTCNT_MASK |
            SPI_PUSHR_PCS(1 << slave.ncs_index) |
            SPI_PUSHR_TXDATA(0x55);


    DMA_CSR_REG(DMA0, dma_ch_tx) = DMA_CSR_DREQ_MASK;
    DMA_CSR_REG(DMA0, dma_ch_rx) = DMA_CSR_DREQ_MASK | DMA_CSR_INTMAJOR_MASK;

    DMA_CERR = DMA_CERR_CERR(dma_ch_rx);
    DMA_CINT = DMA_CINT_CINT(dma_ch_rx);

    DMA_CITER_ELINKNO_REG(DMA0, dma_ch_rx) = DMA_CITER_ELINKNO_CITER(count);
    DMA_BITER_ELINKNO_REG(DMA0, dma_ch_rx) = DMA_BITER_ELINKNO_BITER(count);
    DMA_NBYTES_MLOFFNO_REG(DMA0, dma_ch_rx) = DMA_NBYTES_MLOFFNO_NBYTES(1);

    if(rx_buf){
        DMA_DADDR_REG(DMA0, dma_ch_rx) = (uint32_t)rx_buf;
        DMA_DOFF_REG(DMA0, dma_ch_rx) = 1; // Increment dest!
    } else {
        DMA_DADDR_REG(DMA0, dma_ch_rx) = (uint32_t)&dummy_dest;
        DMA_DOFF_REG(DMA0, dma_ch_rx) = 0; // Don't increment dest!
    }


    DMA_CERR = DMA_CERR_CERR(dma_ch_tx);
    DMA_CINT = DMA_CINT_CINT(dma_ch_tx);

    DMA_CITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_CITER_ELINKNO_CITER(count);
    DMA_BITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_BITER_ELINKNO_BITER(count);
    DMA_NBYTES_MLOFFNO_REG(DMA0, dma_ch_tx) = DMA_NBYTES_MLOFFNO_NBYTES(1);
    DMA_NBYTES_MLOFFYES_REG(DMA0, dma_ch_tx) = DMA_NBYTES_MLOFFYES_NBYTES(1);
    DMA_NBYTES_MLNO_REG(DMA0, dma_ch_tx) = DMA_NBYTES_MLNO_NBYTES(1);

    if(tx_buf){
        // Point to actual buffer
        DMA_SADDR_REG(DMA0, dma_ch_tx) = (uint32_t)tx_buf;
        DMA_SOFF_REG(DMA0, dma_ch_tx) = 1; // Increment source!
    } else {
        // Point to a zero and send that
        DMA_SOFF_REG(DMA0, dma_ch_tx) = 0; // Don't increment source!
        DMA_SADDR_REG(DMA0, dma_ch_tx) = (uint32_t)&zero;
        tx_buf = &zero;
    }

    // Clear SPI flags
    spi->SR =
            SPI_SR_TCF_MASK |
            SPI_SR_TXRXS_MASK |
            SPI_SR_EOQF_MASK |
            SPI_SR_TFUF_MASK |
            SPI_SR_TFFF_MASK |
            SPI_SR_RFOF_MASK |
            SPI_SR_RFDF_MASK;

    // Go SPI!
    spi->MCR &= ~SPI_MCR_HALT_MASK;

    NVIC_EnableIRQ((IRQn_Type)dma_ch_rx);

    // Start receive
    DMA_SSRT = DMA_SSRT_SSRT(dma_ch_rx);



    // Start transmit
    DMA_SSRT = DMA_SSRT_SSRT(dma_ch_tx);



    if(not current_cb){
        // Wait for it!
        while(busy);
    }


    return true;
}
