#include <bsp/codec.h>

AK4621::sample_t AK4621::buffer_in[buffer_size * 2];
AK4621::sample_t AK4621::buffer_out[buffer_size * 2];

void (*AK4621::cb_in)(sample_t *) = nullptr;
void (*AK4621::cb_out)(sample_t *) = nullptr;

uint_fast8_t AK4621::rx_buffer_sel;
uint_fast8_t AK4621::tx_buffer_sel;

void AK4621::init(){
	__disable_irq();
	SIM_SCGC6 |= SIM_SCGC6_I2S_MASK | SIM_SCGC6_DMAMUX_MASK;
	if(SPI == SPI0_BASE_PTR){
		SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;
	} else if(SPI == SPI1_BASE_PTR){
		SIM_SCGC6 |= SIM_SCGC6_SPI1_MASK;
	} else {
		while(true);
	}

	SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;

	gpio_init();

	spi_init();


	for(uint32_t volatile a = 0xFFFF; a; a--);
	// Turn on
	PDN.set();

	cb_in = nullptr;
	cb_out = nullptr;

	__enable_irq();

	for(auto &i : buffer_in) i = 0xA5A51111;
	//for(auto &i : buffer_out) i = 0xFF00FF00;
	for(uint32_t i = 0; i < buffer_size * 2; i++){
		if(i & 0x10){
			buffer_out[i] = 0xFF00FF00;
		} else {
			buffer_out[i] = 0x00FF00FF;
		}
	}
	
	i2s_init();

	for(uint32_t volatile a = 0xFFFF; a; a--);

	spi_write_reg(REG_RESET, 0x00); // Reset
	spi_write_reg(REG_CLK_FORMAT, 0x40); // MCLK = 256fs, fs = 48k
	//spi_write_reg(REG_DEEM_VOL, 0x02); // 48k de-emph

	//spi_write_reg(REG_POWER_DOWN, 0x07); // These are defaults...

	spi_write_reg(REG_RESET, 0x03); // Enable stuff
	
	i2s_dma_init();
}

void AK4621::gpio_init(){
	PDN.configure(GPIOPin::MUX_GPIO, true);
	PDN.make_output();
	PDN.clear();

	MOSI.configure(MOSI_mux);
	SCLK.configure(SCLK_mux);
	NCS.configure(NCS_mux);

	MCLK.configure(MCLK_mux, true, false, false, true, false, false);
	LRCK.configure(LRCK_mux, true);
	BCLK.configure(BCLK_mux, true);
	SDO.configure(SDO_mux, true);
	SDIN.configure(SDIN_mux, true);
}

void AK4621::spi_init(){
	SPI->MCR = SPI_MCR_HALT_MASK;

	/*
	 So the AK4621 specifies that the first clock edge it sees is rising and
	 that it will register on that rising edge. CPOL=CPHA=0
	 */
	SPI->CTAR[0] =
			SPI_CTAR_FMSZ(15) |     // 16 bit frames
			SPI_CTAR_CPOL_MASK |   // Clock idle high
			SPI_CTAR_CPHA_MASK |   // Sample following edge
			SPI_CTAR_PCSSCK(2) |   // PCS to SCK prescaler = 5
			SPI_CTAR_PASC(2)   |   // Same delay before end of PCS
			SPI_CTAR_PDT(3)    |   // Same delay after end of PCS
			SPI_CTAR_PBR(0)    |   // Use sysclk / 2
			SPI_CTAR_CSSCK(6)  |   // Base delay is 128*Tsys
			SPI_CTAR_ASC(3)    |   // Unit delay before end of PCS
			SPI_CTAR_DT(3)     |   // Same after end of PCS
			SPI_CTAR_BR(6);        // Scale by 64

	SPI->MCR =
			SPI_MCR_MSTR_MASK |    // Master
			SPI_MCR_DCONF(0) |     // SPI mode
			SPI_MCR_PCSIS(1) |     // All nCS idle high
			SPI_MCR_DIS_RXF_MASK | // No RX FIFO
			SPI_MCR_CLR_TXF_MASK | // Clear TX FIFO
			SPI_MCR_SMPL_PT(0);    // Sample on clock edge
}

void AK4621::spi_write_reg(reg_t reg, uint8_t value){
	while((SPI->SR & SPI_SR_TFFF_MASK) == 0);
	SPI->PUSHR = SPI_PUSHR_TXDATA(((0xA0 | reg) << 8) | value) |
					SPI_PUSHR_PCS(1) | SPI_PUSHR_EOQ_MASK;
	while((SPI->SR & SPI_SR_EOQF_MASK) == 0); // Wait for end of queue
	SPI->SR = SPI_SR_EOQF_MASK; // Clear flag
}

void AK4621::i2s_init(){
	////////////////
	/// MCLK
	////////////////

	I2S->MDR =
			I2S_MDR_FRACT(mclk_gen_frac - 1) |
			I2S_MDR_DIVIDE(mclk_gen_div - 1);

	I2S->MCR =
			I2S_MCR_MOE_MASK | // Enable MCLK output
			I2S_MCR_MICS(0);   // Use PLL for input - could use 0 (sys)

	I2S->RCSR = I2S_RCSR_SR_MASK;
	I2S->TCSR = I2S_TCSR_SR_MASK;

	////////////////
	/// RECEIVER
	////////////////
	
	I2S->RMR = 0;
	
	I2S->RCR1 =
		I2S_RCR1_RFW(nwords - 1);    // FIFO watermark

	I2S->RCR2 =                 // Receiver configuration
		I2S_RCR2_SYNC(1) |      // Sync mode with transmitter
		I2S_RCR2_BCP_MASK |     // Sample on rising BCLK
		I2S_RCR2_MSEL(0) |
		I2S_RCR2_DIV((mclk_hz / bclk_hz) / 2 - 1) |
		I2S_RCR2_BCD_MASK;      // Master mode but bit clock internal?
	
	I2S->RCR3 =                 // Receiver configuration
		I2S_RCR3_RCE(1) |       // Enable channel
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
		I2S_TCR1_TFW(nwords - 1);    // Same as FRSZ


	I2S->TCR2 =                 // Transmit configuration
		I2S_TCR2_SYNC(0) |      // Async mode -- receiver will run off this
		I2S_TCR2_MSEL(1) |      // Derive clock from MCLK
		I2S_TCR2_BCP_MASK |     // Sample on rising BCLK
		I2S_TCR2_BCD_MASK |     // BCLK is output
		I2S_TCR2_DIV((mclk_hz / bclk_hz) / 2 - 1);

	I2S->TCR3 =                 // Transmit configuration
		I2S_TCR3_TCE(1) |       // Enable both channels?
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
	rx_buffer_sel = 0;
	tx_buffer_sel = 0;
	
	DMA_CR =
		DMA_CR_EMLM_MASK |      // Enable minor looping
		DMA_CR_ERCA_MASK;       // Enable RR arbitration
	
	if(enable_dma_rx){
	DMA_TCD1_CSR = 0;
	
	// First configure receive channel
	DMAMUX_CHCFG1 = 0;
	
	NVIC_EnableIRQ(DMA_CH1_IRQn);
	
	DMA_TCD1_SADDR = (uint32_t)&I2S->RDR[0]; // Receive FIFO
	DMA_TCD1_DADDR = (uint32_t)&buffer_in[0]; // Receive FIFO
	DMA_TCD1_SOFF = 0; // Don't increment source
	DMA_TCD1_DOFF = 4; // Increment destination 32 bits
	DMA_TCD1_ATTR = DMA_ATTR_SMOD(0) | DMA_ATTR_SSIZE(2) | // 32-bit src
	                DMA_ATTR_DMOD(0) | DMA_ATTR_DSIZE(2);  // 32-bit dst
	DMA_TCD1_NBYTES_MLNO = 4;
	// CITER and BITER must be the same
	DMA_TCD1_CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(buffer_size * 2);
	DMA_TCD1_BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(buffer_size * 2);
	DMA_TCD1_SLAST = 0;
	
	DMA_TCD1_DLASTSGA = -(buffer_size * 2 * 4);
	
	DMA_TCD1_CSR = DMA_CSR_INTMAJOR_MASK | // Major loop IRQ
	               DMA_CSR_INTHALF_MASK | // Also at half
                   DMA_CSR_BWC(0); // 0-cycle delay
	
	DMA_SERQ = DMA_SERQ_SERQ(1); // Enable ch 1 requests
	
	DMAMUX_CHCFG1 = DMAMUX_CHCFG_SOURCE(I2S_DMAMUX_SOURCE_RX) |
	                DMAMUX_CHCFG_ENBL_MASK;
	
	DMA_TCD1_CSR |= DMA_CSR_ACTIVE_MASK; // Set active
	}
	
	// Then output
	if(enable_dma_tx){
	DMAMUX_CHCFG0 = 0;
	
	NVIC_EnableIRQ(DMA_CH0_IRQn);
	
	DMA_TCD0_DADDR = (uint32_t)&I2S->TDR[0]; // Transmit FIFO
	DMA_TCD0_SADDR = (uint32_t)&buffer_out[0];
	DMA_TCD0_SOFF = 4; // Increment source!
	DMA_TCD0_DOFF = 0; // Don't increment destination!
	DMA_TCD0_ATTR = DMA_ATTR_SMOD(0) | DMA_ATTR_SSIZE(2) | // 32-bit src
	                DMA_ATTR_DMOD(0) | DMA_ATTR_DSIZE(2);  // 32-bit dst
	DMA_TCD0_NBYTES_MLNO = 4;
	DMA_TCD0_NBYTES_MLOFFNO = 4;
	DMA_TCD0_NBYTES_MLOFFYES = 4;
	// CITER and BITER must be the same
	DMA_TCD0_CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(buffer_size * 2);
	DMA_TCD0_BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(buffer_size * 2);
	DMA_TCD0_SLAST = -(buffer_size * 2 * 4);
	
	DMA_TCD0_DLASTSGA = 0;

	DMA_TCD0_CSR = DMA_CSR_INTMAJOR_MASK | // Major loop IRQ
	               DMA_CSR_INTHALF_MASK | // Also at half
                   DMA_CSR_BWC(0); // 0-cycle delay
	
	DMA_SERQ = DMA_SERQ_SERQ(0); // Enable ch 0 requests
	
	DMAMUX_CHCFG0 = DMAMUX_CHCFG_SOURCE(I2S_DMAMUX_SOURCE_TX) |
	                DMAMUX_CHCFG_ENBL_MASK;
	
	DMA_TCD0_CSR |= DMA_CSR_ACTIVE_MASK; // Set actuve
	}
}

void AK4621::start(){
	if(cb_out)
		cb_out(&buffer_out[0]);
	if(cb_out)
		cb_out(&buffer_out[buffer_size]);
	
	I2S->RCSR |=
		I2S_RCSR_FEF_MASK |
		I2S_RCSR_SEF_MASK |
		I2S_RCSR_FRF_MASK;
	
	I2S->TCSR |=
		I2S_TCSR_FEF_MASK |
		I2S_TCSR_SEF_MASK |
		I2S_TCSR_FRF_MASK;
	
	if(enable_dma_rx){
	DMA_CERR = DMA_CERR_CERR(1);
	DMA_CINT = DMA_CINT_CINT(1);
	
	// Start receive
	DMA_SSRT = DMA_SSRT_SSRT(1);
	}
	
	if(enable_dma_tx){
	DMA_CERR = DMA_CERR_CERR(0);
	DMA_CINT = DMA_CINT_CINT(0);
	
	// Start transmit
	DMA_SSRT = DMA_SSRT_SSRT(0);
	}	
}


void DMA_CH1_ISR() { // Receive
	static uint32_t count = 0;
	count += 1;
	AK4621::dma_rx_isr();
}

void DMA_CH0_ISR() { // Transmit
    AK4621::dma_tx_isr();
}