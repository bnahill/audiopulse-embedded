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

 @file
 @brief
 @author Ben Nahill <bnahill@gmail.com>
 */

#ifndef __APULSE_CODEC_H_
#define __APULSE_CODEC_H_

#include <derivative.h>

#if __cplusplus

#include <stdint.h>
#include <driver/gpio.h>
#include "clocks.h"
#include <apulse_math.h>

class AK4621 {
public:
	//! A single audio sample
	typedef sFractional<0,31> sample_t;
	//! A callback for handling either unused or full data buffers
	typedef void (*audio_cb_t)(sample_t *, size_t);
	typedef void (*audio_out_cb_t)(sample_t *, size_t, uint32_t);

	enum class Src : uint8_t {
		MIC = 0,
		EXT = 1,
		MIX = 2,
	};

	static bool is_Src(Src val){
		return (val == Src::MIC) || (val == Src::EXT) || (val == Src::MIX);
	}

	/*!
	 @brief Initialize all local hardware and software elements for full-duplex stereo
	 audio

	 This involves initializing GPIO pins, SPI, I2S, and DMA. I2S streaming by
	 DMA will not begin until \ref start() is called.

	 @post Ready to call init()
	 */
	static void init_hw();

	/*!
	 @brief Configure the actual device
	 @pre init_hw() has been called
	 @pre Power has been enabled for the analog circuitry

	 @note This must be called after any loss of power on analog circuitry
	 */
	static void init();

	/*!
	 @brief Set callback for new input data

	 The function provided will be called with a pointer to the new data, which
	 will be of length \ref in_buffer_size
	 */
	static void set_in_cb(audio_cb_t new_cb){cb_in = new_cb;}

	/*!
	 @brief Set callback for free output buffer

	 The function provided will be called with a pointer to the free buffer,
	 which will be of length \ref out_buffer_size

	 @note If called before starting output, both buffers will be presented for
	 population before the stream begins
	 */
	static void set_out_cb(audio_out_cb_t new_cb){cb_out= new_cb;}

	/*!
	 @brief Interrupt service routine for outgoing data buffer empty

	 @warning Please don't actually call this unless from interrupt context
	 */
	static void dma_tx_isr(){
		if(cb_out){
			if(enable_loopback){

			} else {
				cb_out(&buffer_out[out_buffer_size * tx_buffer_sel],
					   0, 2);
			}
			tx_buffer_sel ^= 1;
		}
		DMA_CINT = DMA_CINT_CINT(0);
		NVIC_ClearPendingIRQ(DMA_CH0_IRQn);
	}

	/*!
	 @brief Interrupt service routine for new incoming data

	 @warning Please don't actually call this unless from interrupt context
	 */
	static void dma_rx_isr(){
		if(cb_in){
			sample_t * buffer =  &buffer_in[in_buffer_size * rx_buffer_sel];
			if(enable_loopback){
				// Overwrite input data with output data
				if(cb_out){
					for(uint32_t i = 0; i < in_buffer_size / (out_buffer_size / 2); i++){
						cb_out(buffer + (i * out_buffer_size / 2), 0, 1);
					}
					cb_in(buffer, in_buffer_size);
				}
			} else if(source == Src::MIX){
				for(uint32_t i = 0; i < (in_buffer_size / 2); i++){
					sFractional<0,31> m, e;
					m = buffer[2*i];
					e = buffer[2*i + 1];
					m = m * mix_mic + e * mix_ext;
					buffer[i] = sample_t::fromInternal(m.i);
				}
				cb_in(buffer, in_buffer_size / 2);
			} else {
				cb_in(buffer, in_buffer_size);
			}
			rx_buffer_sel ^= 1;
		}

		// Clear the interrupt
		DMA_CINT = DMA_CINT_CINT(1);
		NVIC_ClearPendingIRQ(DMA_CH1_IRQn);
	}

	/*!
	 @brief Start the audio codec streaming

	 @note Right now there is no corresponding 'stop'
	 */
	static void start();
	
	static void stop();

	/*!
	 @brief Select which channels are enabled
	 @param channels The bitwise OR of \ref channels_t to enable

	 @note This may be set before or after start, but will cause some corruption
	 in the current DMA frame only if changed while active

	 @note The channel configuration is initialized to all channels disabled,
	 allowing for the first assignment to occur after initialization.
	 */
	static void set_source(Src new_source,
						   sFractional<0,31> new_mix_mic = (sFractional<0,31>) 0.0,
						   sFractional<0,31> new_mix_ext = (sFractional<0,31>) 0.0){
		AK4621::source = new_source;
		switch(new_source){
		case Src::MIC:
			I2S->RMR = ~1;
			break;
		case Src::EXT:
			I2S->RMR = ~2;
			break;
		case Src::MIX:
			mix_ext = new_mix_ext;
			mix_mic = new_mix_mic;
			I2S->RMR = ~3;
			break;
		}
	}

	/*!
	 @brief The number of samples in a single incoming buffer (there are two)
	 */
	static constexpr uint32_t in_buffer_size = 768*2;

	/*!
	 @brief The number of samples in a single outgoing buffer (there are two)
	 */
	static constexpr uint32_t out_buffer_size = 128;

	static constexpr uint32_t fs = 48000;

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

	static sFractional<0,31> mix_mic, mix_ext;

	static void gpio_init();

	static void spi_init();

	static void spi_write_reg(reg_t reg, uint8_t value);

	static void i2s_init();

	static void i2s_dma_init();

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

	static constexpr GPIOPin XTAL_12288 = {PTB_BASE_PTR, 18};

	static constexpr GPIOPin PDN  = {PTB_BASE_PTR, 3};

	static constexpr I2S_MemMapPtr I2S = I2S0_BASE_PTR;
#if CFG_TARGET_K20
	static constexpr uint32_t I2S_DMAMUX_SOURCE_RX = 14; // p77 of RM
	static constexpr uint32_t I2S_DMAMUX_SOURCE_TX = 15;
#elif CFG_TARGET_K22
	static constexpr uint32_t I2S_DMAMUX_SOURCE_RX = 12; // p78 of RM
	static constexpr uint32_t I2S_DMAMUX_SOURCE_TX = 13;
#endif
	static constexpr IRQn_Type DMA_CH0_IRQn = (IRQn_Type)0;
	static constexpr IRQn_Type DMA_CH1_IRQn = (IRQn_Type)1;

	static constexpr uint32_t word_width = 32;
	static constexpr uint32_t nwords = 2;

	static constexpr uint32_t mclk_hz = 12288000;
	static constexpr uint32_t lrck_hz = 48000;
	static constexpr uint32_t bclk_hz = word_width * nwords * lrck_hz;

	static constexpr uint32_t mclk_gen_frac(){
		return (Clock::config.mcgoutclk == 120000000) ? 192 :
		       ((Clock::config.mcgoutclk == 48000000) ? 32 : 0);
	}
	static constexpr uint32_t mclk_gen_div(){
		return (Clock::config.mcgoutclk == 120000000) ? 1875 :
		       ((Clock::config.mcgoutclk == 48000000) ? 125 : 0);
	}
	//static constexpr uint32_t mclk_gen_frac = 1;
	//static constexpr uint32_t mclk_gen_div = 8;

	static constexpr bool enable_dma_tx = true;
	static constexpr bool enable_dma_rx = true;
	static constexpr bool ext_mclk = true;
	static constexpr bool enable_loopback = CFG_ENABLE_LOOPBACK;

	static uint_fast8_t rx_buffer_sel;
	static uint_fast8_t tx_buffer_sel;

	static Src source;

	//! @name DMA Buffers
	//! @{
	static sample_t buffer_in[in_buffer_size * 2];
	static sample_t buffer_out[out_buffer_size * 2];
	//! @}

	static void (*cb_in)(sample_t *, size_t);
	static void (*cb_out)(sample_t *, size_t, uint32_t);
};

extern "C" {

#endif // __cplusplus

// C prototypes for vector table
void DMA_CH0_ISR();
void DMA_CH1_ISR();

#if __cplusplus
};
#endif // __cplusplus

#endif // __APULSE_CODEC_H_

