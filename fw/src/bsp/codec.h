#ifndef __APULSE_CODEC_H_
#define __APULSE_CODEC_H_

#include <derivative.h>
#include <bsp/gpio.h>

class AK4621 {
public:
	static void init();

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

	typedef uint32_t sample_t;
	typedef void (*audio_cb_t)(sample_t, uint32_t);
	static void set_in_cb(audio_cb_t new_cb);
	static void set_out_cb(audio_cb_t new_cb);
	
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

	static void gpio_init();

	static void spi_init();

	static void spi_write_reg(reg_t reg, uint8_t value);

	static void i2s_init();

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

	static constexpr uint32_t mclk_hz = 12288000;
	static constexpr uint32_t lrck_hz = 48000;
	static constexpr uint32_t bclk_hz = 64 * lrck_hz;

	static constexpr uint32_t mclk_gen_frac = 32;
	static constexpr uint32_t mclk_gen_div = 125;

	//! @name DMA Buffers
	//! @{
	static constexpr uint32_t buffer_size = 128;

	static sample_t buffer_inA[128];
	static sample_t buffer_inB[128];
	static sample_t buffer_outA[128];
	static sample_t buffer_outB[128];
	//! @}

	static void (*cb_in)(sample_t const *);
	static void (*cb_out)(sample_t const *);
};

#endif // __APULSE_CODEC_H_
