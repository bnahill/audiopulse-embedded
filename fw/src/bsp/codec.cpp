#include <bsp/codec.h>

AK4621::sample_t AK4621::buffer_inA[buffer_size];
AK4621::sample_t AK4621::buffer_inB[buffer_size];
AK4621::sample_t AK4621::buffer_outA[buffer_size];
AK4621::sample_t AK4621::buffer_outB[buffer_size];

void (*AK4621::cb_in)(sample_t const *) = nullptr;
void (*AK4621::cb_out)(sample_t const *) = nullptr;

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

	i2s_init();

	for(uint32_t volatile a = 0xFFFF; a; a--);

	spi_write_reg(REG_RESET, 0x00); // Reset
	spi_write_reg(REG_CLK_FORMAT, 0x40); // MCLK = 256fs, fs = 48k
	spi_write_reg(REG_CLK_FORMAT, 0x44); // Don't use this
	//spi_write_reg(REG_DEEM_VOL, 0x02); // 48k de-emph

	//spi_write_reg(REG_POWER_DOWN, 0x07); // These are defaults...

	spi_write_reg(REG_RESET, 0x03); // Enable stuff


	uint32_t i = 0;
	while(true){
		uint32_t tmp = I2S->TFR[0];
		if(I2S->TCSR & I2S_TCSR_FEF_MASK){
			I2S->TCSR |= I2S_TCSR_FEF_MASK;
		}
		if(((tmp & 0x3) != ((tmp >> 16) & 0x3)) ||
		   ((tmp & 0x4) == ((tmp >> 16) & 0x4))
		){
			if(i++ & 1){
				I2S->TDR[0] = 0x0ff0f0;
				I2S->TDR[1] = 0x0ff0f0;
			} else {
				I2S->TDR[0] = 0x000000;
				I2S->TDR[1] = 0x000000;
			}
		}
	}
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

	//I2S->MCR = 0;
	I2S->RCSR = I2S_RCSR_SR_MASK | I2S_RCSR_FR_MASK;
	I2S->TCSR = I2S_TCSR_SR_MASK | I2S_TCSR_FR_MASK;

	////////////////
	/// RECEIVER
	////////////////

	I2S->RCR5 =                 // Receiver configuration
			I2S_RCR5_WNW(32 - 1) |  // Has to be >= W0W
			I2S_RCR5_W0W(32 - 1);   // One 32-bit word

	I2S->RCR4 =                 // Receiver configuration
			I2S_RCR4_FRSZ(1) |  // Two word per frame
			I2S_RCR4_SYWD(32 - 1) | // 32-bit frames
			I2S_RCR4_MF_MASK |  // MSB first
			//I2S_RCR4_FSE_MASK | // Frame sync one bit early -- THIS COULD BE WRONG
			I2S_RCR4_FSP_MASK | // FS active high; figure this out later
			I2S_RCR4_FSD_MASK;  // Master mode but frame clock internal?

	I2S->RCR3 =                 // Receiver configuration
			I2S_RCR3_RCE(3) |   // Enable channel
			I2S_RCR3_WDFL(1);   // Send once each channel has a frame?


	I2S->RCR2 =                 // Receiver configuration
			I2S_RCR2_SYNC(1) |  // Sync mode with transmitter
			I2S_RCR2_BCP_MASK | // Sample on rising BCLK
			I2S_RCR2_BCD_MASK;  // Master mode but bit clock internal?

	////////////////
	/// TRANSMITTER
	////////////////

	I2S->TMR = 0;

	I2S->TCR1 =
		I2S_TCR1_TFW(2 - 1);    // Same as FRSZ


	I2S->TCR2 =                 // Transmit configuration
		I2S_TCR2_SYNC(0) |      // Async mode -- receiver will run off this
		I2S_TCR2_MSEL(1) |      // Derive clock from MCLK
		I2S_TCR2_BCP_MASK |     // Sample on rising BCLK
		I2S_TCR2_BCD_MASK |     // BCLK is output
		I2S_TCR2_DIV((mclk_hz / bclk_hz) / 2 - 1);

	I2S->TCR3 =                 // Transmit configuration
		I2S_TCR3_TCE(0b11) |    // Enable both channels?
		I2S_TCR3_WDFL(0);       // NO IDEA, but some examples do this

	I2S->TCR4 =                 // Transmit configuration
		I2S_TCR4_FRSZ(2 - 1) |  // Two word per frame
		I2S_TCR4_SYWD(32 - 1) | // 32-bit frames
		I2S_TCR4_MF_MASK |      // MSB first
		I2S_TCR4_FSE_MASK |     // Frame sync one bit early -- THIS SHOULD BE GOOD
		I2S_TCR4_FSD_MASK;      // Generate FS

	I2S->TCR5 =                 // Transmit configuration
		I2S_TCR5_FBT(32 - 1) |  // Should maybe be 23...
		I2S_TCR5_WNW(32 - 1) |  // Has to be >= W0W
		I2S_TCR5_W0W(32 - 1);   // One 24-bit word

	I2S->RCSR =
		I2S_RCSR_RE_MASK |      // Receive enable
		I2S_RCSR_DBGE_MASK |    // Continue on debug halt
//		I2S_RCSR_BCE_MASK |     // Enable bit clock transmit
		I2S_RCSR_FRDE_MASK;     // Enable FIFO DMA request

	I2S->TCSR =
		I2S_TCSR_TE_MASK |      // Transmit enable
		I2S_TCSR_DBGE_MASK |    // Continue on debug halt
		I2S_TCSR_BCE_MASK |     // Enable bit clock transmit
		I2S_TCSR_FRDE_MASK;     // Enable FIFO DMA request

// 	DMA_TCD0_SADDR = (uint32_t)&I2S->TFR[0];
// 	DMA_TCD0_SOFF = 0;
// 	DMA_TCD0_ATTR = 0x202; //Source and destination size 32bit, no modulo
// 	DMA_TCD0_NBYTES_MLNO = 0x4;
// 	DMA_TCD0_NBYTES_MLOFFNO = 0x4;
// 	DMA_TCD0_NBYTES_MLOFFYES = 0x4;
// 	DMA_TCD0_SLAST = 0;
//
// 	DMAMUX_CHCFG0 = 0;
}
