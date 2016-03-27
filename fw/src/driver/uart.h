#ifndef __APULSE_UART_H
#define __APULSE_UART_H

#include <derivative.h>
#include <driver/gpio.h>
#include <driver/clocks.h>

class UART {
public:
    UART(UART_MemMapPtr dev, GPIOPin const &rx, GPIOPin const &tx,
         uint32_t dma_ch_tx) :
        dev(dev), rx(rx), tx(tx), is_init(false), dma_ch_tx(dma_ch_tx)
    {

    }

    bool init(uint32_t br_target){
        if(is_init)
            return true;

        rx.configure(GPIOPin::MUX_ALT3);
        tx.configure(GPIOPin::MUX_ALT3);

        // The value to be used in generating the module clock
        float divider = Clock::config.mcgoutclk * 32 / (br_target * 16);
        uint16_t br_reg = divider / 32;
        uint16_t fine_adjust = ((uint32_t)divider) % 32;

        dev->BDH = UART_BDH_SBR(br_reg >> 5);
        dev->BDL = UART_BDL_SBR(br_reg & 0x1FFF);
        dev->C4 = UART_C4_BRFA(fine_adjust);

        dev->C1 =
            UART_C1_M_MASK;
        dev->C2 =
            UART_C2_TIE_MASK |
            UART_C2_TE_MASK;
        dev->C5 =
            UART_C5_TDMAS_MASK;

        DMA_DADDR_REG(DMA0, dma_ch_tx) = (uint32_t)&dev->D; // Transmit FIFO
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

        if(dev == UART0_BASE_PTR){
            DMAMUX_CHCFG_REG(DMAMUX, dma_ch_tx) = DMAMUX_CHCFG_SOURCE(3) |
                                                  DMAMUX_CHCFG_ENBL_MASK;
            //DMAMUX_CHCFG_REG(DMAMUX, dma_ch_rx) = DMAMUX_CHCFG_SOURCE(2) |
            //                                      DMAMUX_CHCFG_ENBL_MASK;
        } else if(dev == UART1_BASE_PTR){
            DMAMUX_CHCFG_REG(DMAMUX, dma_ch_tx) = DMAMUX_CHCFG_SOURCE(5) |
                                                  DMAMUX_CHCFG_ENBL_MASK;
            //DMAMUX_CHCFG_REG(DMAMUX, dma_ch_rx) = DMAMUX_CHCFG_SOURCE(4) |
            //                                      DMAMUX_CHCFG_ENBL_MASK;
        } else {
            while(true);
        }

        return true;
    }

    void send_data(uint8_t const * src){

    }

private:
    bool is_init;
    GPIOPin const rx, tx;
    uint32_t dma_ch_tx;

    UART_MemMapPtr const dev;
};

#endif // __APULSE_UART_H
