#ifndef __APULSE_CODEC_H_
#define __APULSE_CODEC_H_

#include <derivative.h>
#include <bsp/gpio.h>

class AK4621 {
public:
	static void init(){
		__disable_irq();
		SIM_SCGC6 |= SIM_SCGC6_I2S_MASK;
		if(SPI == SPI0_BASE_PTR){
			SIM_SCGC6 |= SIM_SCGC6_SPI0_MASK;
		} else if(SPI == SPI1_BASE_PTR){
			SIM_SCGC6 |= SIM_SCGC6_SPI1_MASK;
		} else {
			while(true);
		}

		gpio_init();

		i2s_init();

		__enable_irq();
	}

protected:
	static void gpio_init(){
		MOSI.configure(MOSI_mux);
		SCLK.configure(SCLK_mux);
		NCS.configure(NCS_mux);

		MCLK.configure(MCLK_mux, true);
		LRCK.configure(LRCK_mux, true);
		BCLK.configure(BCLK_mux, true);
		SDO.configure(SDO_mux, true);
		SDIN.configure(SDIN_mux, true);

		PDN.configure(GPIOPin::MUX_GPIO, true);
		PDN.make_output();
	}

	static void i2s_init(){
		I2S->MDR =
				I2S_MDR_FRACT(mclk_gen_frac - 1) |
				I2S_MDR_DIVIDE(mclk_gen_div - 1);

		I2S->MCR =
				I2S_MCR_MOE_MASK | // Enable MCLK output
				I2S_MCR_MICS(3);   // Use PLL for input - could use 0 (sys)

		////////////////
		/// RECEIVER
		////////////////

		I2S->RCR5 =                 // Receiver configuration
				I2S_RCR5_WNW(24 - 1) |  // Has to be >= W0W
				I2S_RCR5_W0W(24 - 1);   // One 24-bit word

		I2S->RCR4 =                 // Receiver configuration
				I2S_RCR4_FRSZ(0) |  // One word per frame
				I2S_RCR4_SYWD(24 - 1) | // 24-bit frames
				I2S_RCR4_MF_MASK |  // MSB first
				I2S_RCR4_FSE_MASK | // Frame sync one bit early -- THIS COULD BE WRONG
				I2S_RCR4_FSP_MASK;  // FS active high; figure this out later

		I2S->RCR3 =                 // Receiver configuration
				I2S_RCR3_RCE(3) |   // Enable channel
				I2S_RCR3_WDFL(1);   // Send once each channel has a frame?


		I2S->RCR2 =                 // Receiver configuration
				I2S_RCR2_SYNC(1) |  // Sync mode with transmitter
				I2S_RCR2_BCP_MASK;  // Sample on rising BCLK

		I2S->RCSR =
				I2S_RCSR_RE_MASK |  // Transmit enable
				I2S_RCSR_DBGE_MASK |// Continue on debug halt
				I2S_RCSR_BCE_MASK | // Enable bit clock transmit
				I2S_RCSR_FRDE_MASK; // Enable FIFO DMA request

		////////////////
		/// TRANSMITTER
		////////////////


		I2S->TCR5 =                 // Transmit configuration
				I2S_TCR5_WNW(24 - 1) |  // Has to be >= W0W
				I2S_TCR5_W0W(24 - 1);   // One 24-bit word


		I2S->TCR4 =                 // Transmit configuration
				I2S_TCR4_FRSZ(0) |  // One word per frame
				I2S_TCR4_SYWD(24 - 1) | // 24-bit frames
				I2S_TCR4_MF_MASK |  // MSB first
				I2S_TCR4_FSE_MASK | // Frame sync one bit early -- THIS COULD BE WRONG
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
				I2S_TCR2_DIV((mclk_hz / bclk_hz) / 2 - 1);

		I2S->TCSR =
				I2S_TCSR_TE_MASK |  // Transmit enable
				I2S_TCSR_DBGE_MASK |// Continue on debug halt
				I2S_TCSR_BCE_MASK | // Enable bit clock transmit
				I2S_TCSR_FRDE_MASK; // Enable FIFO DMA request
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

	static constexpr uint32_t mclk_hz = 24576000;
	static constexpr uint32_t bclk_hz = 3072000;
	static constexpr uint32_t lrck_hz = 48000;

	static constexpr uint32_t mclk_gen_frac = 64;
	static constexpr uint32_t mclk_gen_div = 125;
};

#endif // __APULSE_CODEC_H_
