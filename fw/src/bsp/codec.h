#ifndef __APULSE_CODEC_H_
#define __APULSE_CODEC_H_

#include <derivative.h>
#include <bsp/gpio.h>

class AK4621 {
public:
	static void init(){
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

		i2s_init();

		spi_init();



		__enable_irq();

		spi_write_reg(REG_RESET, 0x02); // ADC only for now

		spi_write_reg(REG_POWER_DOWN, 0x07);
		spi_write_reg(REG_CLK_FORMAT, 0x40); // MCLK = 256fs, fs = 48k
		spi_write_reg(REG_DEEM_VOL, 0x02); // 48k de-emph
	}


	static void test_read(){
		uint32_t samples[256];
		uint32_t sample;

		for(uint32_t i = 0; i < 256; i++){
			while(true){
				sample = I2S->RFR[0];
				if(sample){
					samples[i] = sample;
					break;
				}
			}
		}
	}
	
protected:
	typedef enum {
		REG_POWER_DOWN  = 0,
		REG_RESET       = 1,
		REG_CLK_FORMAT  = 2,
		REG_DEEM_VOL    = 3,
		REG_LCH_DATT    = 6,
		REG_RCH_DATT    = 7,
		REG_LCH_EXT     = 8,
		REG_RCH_EXT     = 9
	} reg_t;

	static void gpio_init(){
		PDN.configure(GPIOPin::MUX_GPIO, true);
		PDN.make_output();
		PDN.clear();

		MOSI.configure(MOSI_mux);
		SCLK.configure(SCLK_mux);
		NCS.configure(NCS_mux);

		MCLK.configure(MCLK_mux, true);
		LRCK.configure(LRCK_mux, true);
		BCLK.configure(BCLK_mux, true);
		SDO.configure(SDO_mux, true);
		SDIN.configure(SDIN_mux, true);

		// Turn on
		PDN.set();
	}

	static void spi_init(){
		/*
		 So the AK4621 specifies that the first clock edge it sees is rising and
		 that it will register on that rising edge. CPOL=CPHA=0
		 */
		SPI->MCR =
				SPI_MCR_MSTR_MASK |    // Master
				SPI_MCR_DCONF(0) |     // SPI mode
				SPI_MCR_PCSIS(0xF) |   // All nCS idle high
				SPI_MCR_DIS_RXF_MASK | // No RX FIFO
				SPI_MCR_SMPL_PT(0);    // Sample on clock edge


		SPI->CTAR[0] =
				SPI_CTAR_FMSZ(7) |     // 8 bit frames
				// SPI_CTAR_CPOL_MASK |   // Clock idle high
				// SPI_CTAR_CPHA_MASK |   // Sample following edge
				SPI_CTAR_PCSSCK(2) |   // PCS to SCK prescaler = 5
				SPI_CTAR_PASC(2)   |   // Same delay before end of PCS
				SPI_CTAR_PDT(2)    |   // Same delay after end of PCS
				SPI_CTAR_PBR(0)    |   // Use sysclk / 2
				SPI_CTAR_CSSCK(2)  |   // Base delay is 8*Tsys
				SPI_CTAR_ASC(1)    |   // Unit delay before end of PCS
				SPI_CTAR_DT(1)     |   // Same after end of PCS
				SPI_CTAR_BR(4);        // Scale by 16
	}

	static void spi_write_reg(reg_t reg, uint8_t value){
		SPI->PUSHR = reg | SPI_PUSHR_PCS(1) | SPI_PUSHR_CONT_MASK;
		SPI->PUSHR = value | SPI_PUSHR_EOQ_MASK | SPI_PUSHR_PCS(1);
		while((SPI->SR & SPI_SR_EOQF_MASK) == 0); // Wait for end of queue
		SPI->SR = SPI_SR_EOQF_MASK; // Clear flag
	}

	static void i2s_init(){
		I2S->MCR = 0;
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
				I2S_RCR4_FSP_MASK;  // FS active high; figure this out later

		I2S->RCR3 =                 // Receiver configuration
				I2S_RCR3_RCE(3) |   // Enable channel
				I2S_RCR3_WDFL(1);   // Send once each channel has a frame?


		I2S->RCR2 =                 // Receiver configuration
				I2S_RCR2_SYNC(1) |  // Sync mode with transmitter
				I2S_RCR2_BCP_MASK;  // Sample on rising BCLK

		I2S->RCSR =
				I2S_RCSR_RE_MASK |  // Receive enable
				I2S_RCSR_DBGE_MASK |// Continue on debug halt
				//I2S_RCSR_BCE_MASK | // Enable bit clock transmit
				I2S_RCSR_FRDE_MASK; // Enable FIFO DMA request

		////////////////
		/// TRANSMITTER
		////////////////


		I2S->TCR5 =                 // Transmit configuration
				I2S_TCR5_WNW(32 - 1) |  // Has to be >= W0W
				I2S_TCR5_W0W(32 - 1);   // One 24-bit word

		I2S->TCR4 =                 // Transmit configuration
				I2S_TCR4_FRSZ(1) |  // Two word per frame
				I2S_TCR4_SYWD(32 - 1) | // 32-bit frames
				I2S_TCR4_MF_MASK |  // MSB first
				//I2S_TCR4_FSE_MASK | // Frame sync one bit early -- THIS COULD BE WRONG
				I2S_TCR4_FSP_MASK | // FS active high; figure this out later
				I2S_TCR4_FSD_MASK;  // Generate FS

		I2S->TCR3 =                 // Transmit configuration
				I2S_TCR3_TCE(3) |   // Enable both channels?
				I2S_TCR3_WDFL(1);   // Send once each channel has a frame?

		I2S->TCR2 =                 // Transmit configuration
				I2S_TCR2_SYNC(0) |  // Async mode -- receiver will run off this
				I2S_TCR2_MSEL(0) |  // Derive clock from bus clock (48MHz)
				I2S_TCR2_BCP_MASK | // Sample on rising BCLK
				I2S_TCR2_BCD_MASK | // BCLK is output
				I2S_TCR2_DIV((mclk_hz / bclk_hz) * 2 - 1);
									// This derivation is in conflict with
									// documentation, but it works

		I2S->TCSR =
				I2S_TCSR_TE_MASK |  // Transmit enable
				I2S_TCSR_DBGE_MASK |// Continue on debug halt
				I2S_TCSR_BCE_MASK | // Enable bit clock transmit
				I2S_TCSR_FRDE_MASK; // Enable FIFO DMA request

		////////////////
		/// MCLK
		////////////////

		I2S->MDR =
				I2S_MDR_FRACT(mclk_gen_frac - 1) |
				I2S_MDR_DIVIDE(mclk_gen_div - 1);

		I2S->MCR =
				I2S_MCR_MOE_MASK | // Enable MCLK output
				I2S_MCR_MICS(0);   // Use PLL for input - could use 0 (sys)


		DMA_TCD0_SADDR = (uint32_t)&I2S->TFR[0];
		DMA_TCD0_SOFF = 0;
		DMA_TCD0_ATTR = 0x202; //Source and destination size 32bit, no modulo
		DMA_TCD0_NBYTES_MLNO = 0x4;
		DMA_TCD0_NBYTES_MLOFFNO = 0x4;
		DMA_TCD0_NBYTES_MLOFFYES = 0x4;
		DMA_TCD0_SLAST = 0;

		DMAMUX_CHCFG0 = 0;
	}

	static constexpr SPI_MemMapPtr SPI = SPI0_BASE_PTR;
	static constexpr GPIOPin MOSI = {PTD_BASE_PTR, 2};
	static constexpr GPIOPin::mux_t MOSI_mux = GPIOPin::MUX_ALT2;
	static constexpr GPIOPin SCLK = {PTD_BASE_PTR, 1};
	static constexpr GPIOPin::mux_t SCLK_mux = GPIOPin::MUX_ALT2;
	static constexpr GPIOPin NCS = {PTD_BASE_PTR, 0};
	static constexpr GPIOPin::mux_t NCS_mux = GPIOPin::MUX_ALT2;


	static constexpr GPIOPin MCLK = {PTC_BASE_PTR, 6};
	static constexpr GPIOPin::mux_t MCLK_mux = GPIOPin::MUX_ALT6;
	static constexpr GPIOPin LRCK = {PTC_BASE_PTR, 2};
	static constexpr GPIOPin::mux_t LRCK_mux = GPIOPin::MUX_ALT6;
	static constexpr GPIOPin BCLK = {PTC_BASE_PTR, 3};
	static constexpr GPIOPin::mux_t BCLK_mux = GPIOPin::MUX_ALT6;
	static constexpr GPIOPin SDO  = {PTC_BASE_PTR, 1};
	static constexpr GPIOPin::mux_t SDO_mux = GPIOPin::MUX_ALT6;
	static constexpr GPIOPin SDIN = {PTC_BASE_PTR, 5};
	static constexpr GPIOPin::mux_t SDIN_mux = GPIOPin::MUX_ALT4;

	static constexpr GPIOPin PDN  = {PTC_BASE_PTR, 0};

	static constexpr I2S_MemMapPtr I2S = I2S0_BASE_PTR;

	static constexpr uint32_t mclk_hz = 24576000/2;
	static constexpr uint32_t lrck_hz = 48000;
	static constexpr uint32_t bclk_hz = 64 * lrck_hz;

	static constexpr uint32_t mclk_gen_frac = 32;
	static constexpr uint32_t mclk_gen_div = 125;
};

#endif // __APULSE_CODEC_H_
