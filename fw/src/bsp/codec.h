#ifndef __APULSE_CODEC_H_
#define __APULSE_CODEC_H_

#include <derivative.h>

#if __cplusplus

#include <bsp/gpio.h>

class AK4621 {
public:
	//! A single audio sample
	typedef int32_t sample_t;
	//! A callback for handling either unused or full data buffers
	typedef void (*audio_cb_t)(sample_t *);
	
	/*!
	 @brief Initialize all hardware and software elements for full-duplex stereo
	 audio
	 
	 This involves initializing GPIO pins, SPI, I2S, and DMA. I2S streaming by
	 DMA will not begin until \ref start() is called.
	 */
	static void init();
	
	/*!
	 @brief Set callback for new input data
	 
	 The function provided will be called with a pointer to the new data, which
	 will be of length \ref buffer_size
	 */
	static void set_in_cb(audio_cb_t new_cb){cb_in = new_cb;}
	
	/*!
	 @brief Set callback for free output buffer
	 
	 The function provided will be called with a pointer to the free buffer,
	 which will be of length \ref buffer_size
	 
	 @note If called before starting output, both buffers will be presented for
	 population before the stream begins
	 */
	static void set_out_cb(audio_cb_t new_cb){cb_out= new_cb;}
	
	/*!
	 @brief Interrupt service routine for outgoing data buffer empty
	 
	 @warning Please don't actually call this unless from interrupt context
	 */
	static void dma_tx_isr(){
		if(cb_out){
			cb_out(&buffer_out[buffer_size * tx_buffer_sel]);
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
			cb_in(&buffer_in[buffer_size * rx_buffer_sel]);
			rx_buffer_sel ^= 1;
		}
		DMA_CINT = DMA_CINT_CINT(1);
		NVIC_ClearPendingIRQ(DMA_CH1_IRQn);
	}
	
	/*!
	 @brief Start the audio codec streaming
	 
	 @note Right now there is no corresponding 'stop'
	 */
	static void start();
	
	/*!
	 @brief The number of samples stored in each buffer (there are two for each
	 direction...
	 */
	static constexpr uint32_t buffer_size = 512;

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

	static constexpr GPIOPin PDN  = {PTC_BASE_PTR, 0};

	static constexpr I2S_MemMapPtr I2S = I2S0_BASE_PTR;
	static constexpr uint32_t I2S_DMAMUX_SOURCE_RX = 14; // p77 of RM
	static constexpr uint32_t I2S_DMAMUX_SOURCE_TX = 15;
	static constexpr IRQn_Type DMA_CH0_IRQn = (IRQn_Type)0;
	static constexpr IRQn_Type DMA_CH1_IRQn = (IRQn_Type)1;
	
	static constexpr uint32_t word_width = 32;
	static constexpr uint32_t nwords = 2;

	static constexpr uint32_t mclk_hz = 12288000;
	static constexpr uint32_t lrck_hz = 48000;
	static constexpr uint32_t bclk_hz = word_width * nwords * lrck_hz;

	static constexpr uint32_t mclk_gen_frac = 32;
	static constexpr uint32_t mclk_gen_div = 125;
	
	static constexpr bool enable_dma_tx = false;
	static constexpr bool enable_dma_rx = true;
	
	static uint_fast8_t rx_buffer_sel;
	static uint_fast8_t tx_buffer_sel;

	//! @name DMA Buffers
	//! @{
	static sample_t buffer_in[buffer_size * 2];
	static sample_t buffer_out[buffer_size * 2];
	//! @}

	static void (*cb_in)(sample_t *);
	static void (*cb_out)(sample_t *);
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
