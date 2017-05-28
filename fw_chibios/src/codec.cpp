/*!
 (C) Copyright 2013 Ben Nahill

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

 @file codec.cpp
 @brief
 @author Ben Nahill <bnahill@gmail.com>
 */

#include <hal.h>
#include <codec.h>

void AK4621::init_hw() {
    if((&SAI_IN == SAI1) || (&SAI_OUT == SAI1)){
        rccEnableSAI1(0);
    }
    if((&SAI_IN == SAI2) || (&SAI_OUT == SAI2)){
        rccEnableSAI2(0);
    }


    SPIConfig spi_config = {
        NULL,
        PAL_PORT(spi_ncs_line),
        PAL_PAD(spi_ncs_line),
        SPI_CR1_BR_2 | SPI_CR1_BR_1,
        0
    };

#if 0
	SIM_SCGC6 |= SIM_SCGC6_I2S_MASK;

	gpio_init();

    spi.init();
    spi.register_slave(slave);

	for(uint32_t volatile a = 0xFFFF; a; a--);
	// Turn on
	PDN.set();

	//cb_in = nullptr;
	//cb_out = nullptr;

	__enable_irq();

	for(auto &i : buffer_in) i = sample_t::fromInternal(0xA5A51111);
	//for(auto &i : buffer_out) i = 0xFF00FF00;
	for(uint32_t i = 0; i < out_buffer_size * 2; i++){
		if(i & 0x10){
			buffer_out[i] = sample_t::fromInternal(0xFF00FF00);
		} else {
			buffer_out[i] = sample_t::fromInternal(0x00FF00FF);
		}
	}

	i2s_init();

	// WHY?
	for(uint32_t volatile a = 0xFFFF; a; a--);

	i2s_dma_init();
#endif
}


void AK4621::init(){
    static constexpr bool deem_48k = false; // Otherwise off
    static constexpr bool high_pass = true; // Otherwise off

#if 0
	PDN.set();

	spi_write_reg(REG_RESET, 0x00); // Reset
	spi_write_reg(REG_CLK_FORMAT, 0x40); // MCLK = 256fs, fs = 48k
    spi_write_reg(REG_DEEM_VOL, (deem_48k  ? 0x02 : 0x01) |
                                (high_pass ? 0x00 : 0x60));

	//spi_write_reg(REG_POWER_DOWN, 0x07); // These are defaults...

	spi_write_reg(REG_RESET, 0x03); // Enable stuff
#endif
}

void AK4621::gpio_init(){
#if 0
	PDN.configure(GPIOPin::MUX_GPIO, true);
	PDN.make_output();
	PDN.clear();

	XTAL_12288.clear();
	XTAL_12288.make_output();
	XTAL_12288.set_mux(GPIOPin::MUX_GPIO);

	MCLK.configure(MCLK_mux, true, false, false, true, false, false);
	LRCK.configure(LRCK_mux, true);
	BCLK.configure(BCLK_mux, true);
	SDO.configure(SDO_mux, true);
	SDIN.configure(SDIN_mux, true);
#endif
}


void AK4621::spi_write_reg(enum reg reg, uint8_t value){
#if 0
    uint8_t data[2] = {0xA0 | ((uint8_t)reg), value};
    while(!spi.transfer(slave, data, nullptr, 2, nullptr, nullptr));
#endif
}

void AK4621::i2s_init(){
#if 0
	////////////////
	/// MCLK
	////////////////
	
	static_assert(mclk_gen_frac() and (mclk_gen_frac() < 256), "Couldn't get MCLK fractional value");
	static_assert(mclk_gen_div() and (mclk_gen_frac() < 2048), "Couldn't get MCLK divider value");

	if(ext_mclk){
		// Just disable MCLK generation...
		I2S->MDR = 0;
		I2S->MCR = 0;
		// And enable the external oscillator
        XTAL_12288.set();
	} else {
		I2S->MDR =
				I2S_MDR_FRACT(mclk_gen_frac() - 1) |
				I2S_MDR_DIVIDE(mclk_gen_div() - 1);

		I2S->MCR =
				I2S_MCR_MOE_MASK | // Enable MCLK output
				I2S_MCR_MICS(0);   // Use PLL for input - could use 0 (sys)
	}

	I2S->RCSR = I2S_RCSR_SR_MASK;
	I2S->TCSR = I2S_TCSR_SR_MASK;

	////////////////
	/// RECEIVER
	////////////////

	switch(source){
		case Src::MIC:
			I2S->RMR = ~1;
			break;
		case Src::EXT:
			I2S->RMR = ~2;
			break;
		case Src::MIX:
			I2S->RMR = ~3;
			break;
		}
	//I2S->RMR = ~channels;

	I2S->RCR1 =
		I2S_RCR1_RFW(0);        // Always request immediately

	I2S->RCR2 =                 // Receiver configuration
		I2S_RCR2_SYNC(1) |      // Sync mode with transmitter
		I2S_RCR2_BCP_MASK |     // Sample on rising BCLK
		I2S_RCR2_MSEL(0) |
		I2S_RCR2_DIV((mclk_hz / bclk_hz) / 2 - 1) |
		I2S_RCR2_BCD_MASK;      // Master mode but bit clock internal?

	I2S->RCR3 =                 // Receiver configuration
        I2S_RCR3_RCE_MASK |     // Enable channel
		I2S_RCR3_WDFL(0);       // Send once each channel has a frame?

	I2S->RCR4 =                 // Receiver configuration
		I2S_RCR4_FRSZ(1) |      // Two word per frame
		I2S_RCR4_SYWD(word_width - 1) | // 32-bit frames
		I2S_RCR4_MF_MASK |      // MSB first
		(I2S_RCR4_FSE_MASK & 0) |   // Frame sync not one bit early
		(I2S_RCR4_FSP_MASK & 0) |   // FS active low; figure this out later
		I2S_RCR4_FSD_MASK;      // Master mode but frame clock internal?

	I2S->RCR5 =                 // Receiver configuration
		I2S_RCR5_FBT(word_width - 1) |
		I2S_RCR5_WNW(word_width - 1) |  // Has to be >= W0W
		I2S_RCR5_W0W(word_width - 1);   // One 32-bit word


	////////////////
	/// TRANSMITTER
	////////////////

	I2S->TMR = 0;

	I2S->TCR1 =
		I2S_TCR1_TFW(7);        // Maximum watermark (will try to keep full)


	I2S->TCR2 =                 // Transmit configuration
		I2S_TCR2_SYNC(0) |      // Async mode -- receiver will run off this
		I2S_TCR2_MSEL(1) |      // Derive clock from MCLK
		I2S_TCR2_BCP_MASK |     // Sample on rising BCLK
		I2S_TCR2_BCD_MASK |     // BCLK is output
		I2S_TCR2_DIV((mclk_hz / bclk_hz) / 2 - 1);

	I2S->TCR3 =                 // Transmit configuration
		I2S_TCR3_TCE_MASK |       // Enable both channels?
		I2S_TCR3_WDFL(0);       // NO IDEA, but some examples do this

	I2S->TCR4 =                 // Transmit configuration
		I2S_TCR4_FRSZ(nwords - 1) |  // Two word per frame
		I2S_TCR4_SYWD(word_width - 1) | // 32-bit frames
		I2S_TCR4_MF_MASK |      // MSB first
		(I2S_TCR4_FSE_MASK & 0) |     // Frame sync not one bit early
		I2S_TCR4_FSD_MASK;      // Generate FS

	I2S->TCR5 =                 // Transmit configuration
		I2S_TCR5_FBT(word_width - 1) |  // Should maybe be 23...
		I2S_TCR5_WNW(word_width - 1) |  // Has to be >= W0W
		I2S_TCR5_W0W(word_width - 1);   // One 24-bit word

	I2S->RCSR =
		I2S_RCSR_FR_MASK |      // Reset FIFO
		I2S_RCSR_RE_MASK |      // Receive enable
		(I2S_RCSR_DBGE_MASK & 0) |    // Stop on debug halt
		I2S_RCSR_BCE_MASK |     // Enable bit clock transmit
		I2S_RCSR_FRDE_MASK;     // Enable FIFO DMA request

	I2S->TCSR =
		I2S_TCSR_FR_MASK |      // Reset FIFO
		I2S_TCSR_TE_MASK |      // Transmit enable
		(I2S_TCSR_DBGE_MASK & 0) |    // Stop on debug halt
		I2S_TCSR_BCE_MASK |     // Enable bit clock transmit
		I2S_TCSR_FRDE_MASK;     // Enable FIFO DMA request
}

void AK4621::i2s_dma_init(){


	if(enable_dma_rx){
    DMA_CSR_REG(DMA0, dma_ch_rx) = 0;

	// First configure receive channel
    DMAMUX_CHCFG(dma_ch_rx) = 0;

    NVIC_EnableIRQ((IRQn_Type)dma_ch_rx);

    DMA_SADDR_REG(DMA0, dma_ch_rx) = (uint32_t)&I2S->RDR[0]; // Receive FIFO
    DMA_DADDR_REG(DMA0, dma_ch_rx) = (uint32_t)&buffer_in[0]; // Receive FIFO
    DMA_SOFF_REG(DMA0, dma_ch_rx) = 0; // Don't increment source
    DMA_DOFF_REG(DMA0, dma_ch_rx) = 4; // Increment destination 32 bits
    DMA_ATTR_REG(DMA0, dma_ch_rx) = DMA_ATTR_SMOD(0) | DMA_ATTR_SSIZE(2) | // 32-bit src
                                    DMA_ATTR_DMOD(0) | DMA_ATTR_DSIZE(2);  // 32-bit dst
    DMA_NBYTES_MLNO_REG(DMA0, dma_ch_rx) = 4;
    DMA_NBYTES_MLOFFNO_REG(DMA0, dma_ch_rx) = 4;
    DMA_NBYTES_MLOFFYES_REG(DMA0, dma_ch_rx) = 4;
	// CITER and BITER must be the same
    DMA_CITER_ELINKNO_REG(DMA0, dma_ch_rx) = DMA_CITER_ELINKNO_CITER(in_buffer_size * 2);
    DMA_BITER_ELINKNO_REG(DMA0, dma_ch_rx) = DMA_BITER_ELINKNO_BITER(in_buffer_size * 2);
    DMA_SLAST_REG(DMA0, dma_ch_rx) = 0;

    DMA_DLAST_SGA_REG(DMA0, dma_ch_rx) = -(in_buffer_size * 2 * 4);

    DMA_CSR_REG(DMA0, dma_ch_rx) = DMA_CSR_INTMAJOR_MASK | // Major loop IRQ
				   DMA_CSR_INTHALF_MASK | // Also at half
				   DMA_CSR_BWC(0); // 0-cycle delay

    DMA_SERQ = DMA_SERQ_SERQ(dma_ch_rx); // Enable ch 1 requests

    DMAMUX_CHCFG(dma_ch_rx) = DMAMUX_CHCFG_SOURCE(I2S_DMAMUX_SOURCE_RX) |
                              DMAMUX_CHCFG_ENBL_MASK;

    DMA_CSR_REG(DMA0, dma_ch_rx) |= DMA_CSR_ACTIVE_MASK; // Set active
	}

	// Then output
	if(enable_dma_tx){
    DMAMUX_CHCFG(dma_ch_tx) = 0;

    NVIC_EnableIRQ((IRQn_Type)dma_ch_tx);

    DMA_DADDR_REG(DMA0, dma_ch_tx) = (uint32_t)&I2S->TDR[0]; // Transmit FIFO
    DMA_SADDR_REG(DMA0, dma_ch_tx) = (uint32_t)&buffer_out[0];
    DMA_SOFF_REG(DMA0, dma_ch_tx) = 4; // Increment source!
    DMA_DOFF_REG(DMA0, dma_ch_tx) = 0; // Don't increment destination!
    DMA_ATTR_REG(DMA0, dma_ch_tx) = DMA_ATTR_SMOD(0) | DMA_ATTR_SSIZE(2) | // 32-bit src
					DMA_ATTR_DMOD(0) | DMA_ATTR_DSIZE(2);  // 32-bit dst
    DMA_NBYTES_MLNO_REG(DMA0, dma_ch_tx) = 4;
    DMA_NBYTES_MLOFFNO_REG(DMA0, dma_ch_tx) = 4;
    DMA_NBYTES_MLOFFYES_REG(DMA0, dma_ch_tx) = 4;
	// CITER and BITER must be the same
    DMA_CITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_CITER_ELINKNO_CITER(out_buffer_size * 2);
    DMA_BITER_ELINKNO_REG(DMA0, dma_ch_tx) = DMA_BITER_ELINKNO_BITER(out_buffer_size * 2);
    DMA_SLAST_REG(DMA0, dma_ch_tx) = -(out_buffer_size * 2 * 4);

    DMA_DLAST_SGA_REG(DMA0, dma_ch_tx) = 0;

    DMA_CSR_REG(DMA0, dma_ch_tx) = DMA_CSR_INTMAJOR_MASK | // Major loop IRQ
				   DMA_CSR_INTHALF_MASK | // Also at half
				   DMA_CSR_BWC(0); // 0-cycle delay

    DMA_SERQ = DMA_SERQ_SERQ(dma_ch_tx); // Enable ch 0 requests

    DMAMUX_CHCFG(dma_ch_tx) = DMAMUX_CHCFG_SOURCE(I2S_DMAMUX_SOURCE_TX) |
                              DMAMUX_CHCFG_ENBL_MASK;

    DMA_CSR_REG(DMA0, dma_ch_tx) |= DMA_CSR_ACTIVE_MASK; // Set actuve
	}
#endif
}

void AK4621::start(){
#if 0
    i2s_init();
    i2s_dma_init();
	rx_buffer_sel = 0;
	tx_buffer_sel = 0;
	if(cb_out){
		cb_out(&buffer_out[0], out_buffer_size, 2);
		cb_out(&buffer_out[out_buffer_size], out_buffer_size, 2);
	}

	I2S->RCSR |=
		I2S_RCSR_FEF_MASK |
		I2S_RCSR_SEF_MASK |
		I2S_RCSR_FRF_MASK;

	I2S->TCSR |=
		I2S_TCSR_FEF_MASK |
		I2S_TCSR_SEF_MASK |
		I2S_TCSR_FRF_MASK;

	if(enable_dma_rx){
        DMA_CERR = DMA_CERR_CERR(dma_ch_rx);
        DMA_CINT = DMA_CINT_CINT(dma_ch_rx);

		// Start receive
        DMA_SSRT = DMA_SSRT_SSRT(dma_ch_rx);
	}

	if(enable_dma_tx){
        DMA_CERR = DMA_CERR_CERR(dma_ch_tx);
        DMA_CINT = DMA_CINT_CINT(dma_ch_tx);

		// Start transmit
        DMA_SSRT = DMA_SSRT_SSRT(dma_ch_tx);
	}
#endif
}

void AK4621::stop(){
#if 0
	if(enable_dma_rx){
        DMA_EARS = 1 << dma_ch_rx;
	}
	if(enable_dma_tx){
        DMA_EARS = 1 << dma_ch_tx;
	}
	if(enable_dma_rx){
        while(DMA_CSR_REG(DMA0, dma_ch_rx) & DMA_CSR_DREQ_MASK);
	}
	if(enable_dma_tx){
        while(DMA_CSR_REG(DMA0, dma_ch_tx) & DMA_CSR_DREQ_MASK);
	}
#endif
}

static volatile uint32_t dma_rx_isr_count = 0;
static volatile uint32_t dma_tx_isr_count = 0;

