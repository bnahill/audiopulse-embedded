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
#include <apulse_util.h>

void AK4621::init_hw() {
	gpio_init();

    chThdSleepMilliseconds(1);
	// Turn on


    cb_in = nullptr;
    cb_out = nullptr;

    //__enable_irq();
    /*
	for(auto &i : buffer_in) i = sample_t::fromInternal(0xA5A51111);
	//for(auto &i : buffer_out) i = 0xFF00FF00;
	for(uint32_t i = 0; i < out_buffer_size * 2; i++){
		if(i & 0x10){
			buffer_out[i] = sample_t::fromInternal(0xFF00FF00);
		} else {
			buffer_out[i] = sample_t::fromInternal(0x00FF00FF);
		}
	}
    */

    i2s_init();
}


void AK4621::init(){
    static constexpr bool deem_48k = false; // Otherwise off
    static constexpr bool high_pass = true; // Otherwise off

    palSetLine(pdn_line);

	spi_write_reg(REG_RESET, 0x00); // Reset
	spi_write_reg(REG_CLK_FORMAT, 0x40); // MCLK = 256fs, fs = 48k
    spi_write_reg(REG_DEEM_VOL, (deem_48k  ? 0x02 : 0x01) |
                                (high_pass ? 0x00 : 0x60));

	//spi_write_reg(REG_POWER_DOWN, 0x07); // These are defaults...

	spi_write_reg(REG_RESET, 0x03); // Enable stuff
}

void AK4621::gpio_init(){
    palClearLine(pdn_line);
    palClearLine(clk_en_line);
}


void AK4621::spi_write_reg(enum reg reg, uint8_t value){
    //uint8_t const data[2] = {((uint8_t)0xA0) | ((uint8_t)reg), value};
    uint8_t data[2] = {0};//(0xA0 | reg), value};
    data[0] = 0xA0 | reg;
    data[1] = value;

    SPIConfig spi_config = {
        NULL,
        PAL_PORT(spi_ncs_line),
        narrow_cast< uint16_t, uint32_t >(PAL_PAD(spi_ncs_line)),
        SPI_CR1_BR_2 | SPI_CR1_BR_1,
        SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0
    };

    spiAcquireBus(&SPI);
    spiStart(&SPI, &spi_config);
    spiSelect(&SPI);
    spiSend(&SPI, sizeof(data), data);
    spiUnselect(&SPI);
    spiReleaseBus(&SPI);
}

void AK4621::i2s_init(){
    palSetLine(clk_en_line);

    SAI_Block_TypeDef * in_block = reinterpret_cast<SAI_Block_TypeDef *>((uint8_t *)&SAI_IN + 4 + sai_in_ch * 0x20);
    SAI_Block_TypeDef * out_block = reinterpret_cast<SAI_Block_TypeDef *>((uint8_t *)&SAI_OUT + 4 + sai_out_ch * 0x20);

    auto dmain = STM32_DMA_STREAM(dma_stream_rx);
    auto dmaout = STM32_DMA_STREAM(dma_stream_tx);

    auto channel = STM32_DMA_GETCHANNEL(dma_stream_rx,                        \
                                        dma_channel_rx);

    dma_mode_rx = STM32_DMA_CR_CHSEL(channel) |
                  STM32_DMA_CR_PL(dma_prio_rx) |
                  STM32_DMA_CR_DIR_P2M |
                  STM32_DMA_CR_TCIE |
                  STM32_DMA_CR_DMEIE |
                  STM32_DMA_CR_TEIE |
                  STM32_DMA_CR_TCIE |
                  STM32_DMA_CR_HTIE |
                  STM32_DMA_CR_CIRC |
                  STM32_DMA_CR_MINC |
                  STM32_DMA_CR_MSIZE_WORD |
                  STM32_DMA_CR_PSIZE_WORD;

    channel = STM32_DMA_GETCHANNEL(dma_stream_tx,                        \
                                   dma_channel_tx);

    dma_mode_tx = STM32_DMA_CR_CHSEL(channel) |
                  STM32_DMA_CR_PL(dma_prio_tx) |
                  STM32_DMA_CR_DIR_M2P |
                  STM32_DMA_CR_DMEIE |
                  STM32_DMA_CR_TEIE |
                  STM32_DMA_CR_TCIE |
                  STM32_DMA_CR_HTIE |
                  STM32_DMA_CR_CIRC |
                  STM32_DMA_CR_MINC |
                  STM32_DMA_CR_MSIZE_WORD |
                  STM32_DMA_CR_PSIZE_WORD;

    auto b = dmaStreamAllocate(dmain, dma_irq_prio_rx, data_in_handler, this);
    osalDbgAssert(b, "Couldn't allocate rx DMA stream");

    b = dmaStreamAllocate(dmaout, dma_irq_prio_tx, data_out_handler, this);
    osalDbgAssert(b, "Couldn't allocate tx DMA stream");

    dmaStreamSetPeripheral(dmain, &in_block->DR);
    dmaStreamSetPeripheral(dmaout, &out_block->DR);
    dmaStreamSetMemory0(dmain, buffer_in);
    dmaStreamSetMemory0(dmaout, buffer_out);

    if((&SAI_IN == SAI1) || (&SAI_OUT == SAI1)){
        rccEnableSAI1(0);
    }
    if((&SAI_IN == SAI2) || (&SAI_OUT == SAI2)){
        rccEnableSAI2(0);
    }

    // Setup IN (SAI2), for it will provide clock to OUT
    // Clocks go from SAI_IN to SAI_OUT
    SAI_IN.GCR = sai_in_ch ? rv(SAI_GCR_SYNCOUT, 2) : rv(SAI_GCR_SYNCOUT, 1);
    SAI_OUT.GCR = (&SAI_IN == SAI1) ? rv(SAI_GCR_SYNCIN, 0) : rv(SAI_GCR_SYNCIN, 1);

    RCC->DCKCFGR1 = rv(RCC_DCKCFGR1_SAI1SEL, 3) | // This should be pulling from I2SCLK directly
                    rv(RCC_DCKCFGR1_SAI2SEL, 3) |
                    rv(RCC_DCKCFGR1_PLLSAIDIVQ, 0) |
                    rv(RCC_DCKCFGR1_PLLI2SDIVQ, 0);
    in_block->CR1 = rv(SAI_xCR1_NODIV, 0) | // No MCK divider
                    rv(SAI_xCR1_MCKDIV, mclk_hz / (bclk_hz * 2)) | // No MCK divider
                    rv(SAI_xCR1_DMAEN, 0) | // No DMA signals yet
                    rv(SAI_xCR1_SAIEN, 0) | // Don't enable yet
                    rv(SAI_xCR1_OUTDRIV, 1) | // Drive output signals
                    rv(SAI_xCR1_MONO, 0) |
                    rv(SAI_xCR1_SYNCEN, 0) | // This block is asynchronous (other will be sync)
                    rv(SAI_xCR1_CKSTR, 1) | // Sample on rising edge
                    rv(SAI_xCR1_LSBFIRST, 0) | // Codec is configured for MSB first
                    rv(SAI_xCR1_DS, 6) | // 24-bit data
                    rv(SAI_xCR1_PRTCFG, 0) | // Free protocol mode
                    rv(SAI_xCR1_MODE, 1); // Master receiver mode
    in_block->CR1 |= rv(SAI_xCR1_DMAEN, 1);
    in_block->CR2 = rv(SAI_xCR2_COMP, 0) | // No companding
                    rv(SAI_xCR2_CPL, 1) | // 2s complement
                    rv(SAI_xCR2_MUTECNT, 0) |
                    rv(SAI_xCR2_MUTEVAL, 0) |
                    rv(SAI_xCR2_MUTE, 0) | // Don't mute
                    rv(SAI_xCR2_TRIS, 0) | // Don't tri-state the data lines
                    rv(SAI_xCR2_FFLUSH, 0) | // Don't
                    rv(SAI_xCR2_FTH, 2); // FIFO threshold at 1/2. Might not use...

    in_block->FRCR = rv(SAI_xFRCR_FSOFF, 0) | // Assert fs on bit 0
                     rv(SAI_xFRCR_FSPOL, 1) | // Rise on frame start
                     rv(SAI_xFRCR_FSDEF, 1) | // fs is sync and channel id (stereo)
                     rv(SAI_xFRCR_FSALL, 23) | // fs is up for 24 bit clocks
                     rv(SAI_xFRCR_FRL, 47); // Overall frame is 48 bit clocks
    in_block->SLOTR = rv(SAI_xSLOTR_SLOTEN, 0b11) | // Enable first 2 channels
                      rv(SAI_xSLOTR_NBSLOT, 1) | // Enable a total of 2 channels
                      rv(SAI_xSLOTR_SLOTSZ, 0) | // Slot size is data size
                      rv(SAI_xSLOTR_FBOFF, 0); // No extra bits at start

    in_block->IMR = 0; // No interrupts


    // Out
    out_block->CR1 = rv(SAI_xCR1_NODIV, 1) | // No MCK divider
                     rv(SAI_xCR1_DMAEN, 0) | // No DMA signals yet
                     rv(SAI_xCR1_SAIEN, 0) | // Don't enable yet
                     rv(SAI_xCR1_OUTDRIV, 1) | // Drive output signals
                     rv(SAI_xCR1_MONO, 0) |
                     rv(SAI_xCR1_SYNCEN, 2) | // This block is synchronous to another SAI
                     rv(SAI_xCR1_CKSTR, 1) | // Sample on rising edge
                     rv(SAI_xCR1_LSBFIRST, 0) | // Codec is configured for MSB first
                     rv(SAI_xCR1_DS, 6) | // 24-bit data
                     rv(SAI_xCR1_PRTCFG, 0) | // Free protocol mode
                     rv(SAI_xCR1_MODE, 2); // Slave transmitter
    out_block->CR1 |= rv(SAI_xCR1_DMAEN, 1);
    out_block->CR2 = rv(SAI_xCR2_COMP, 0) | // No companding
                     rv(SAI_xCR2_CPL, 1) | // 2s complement
                     rv(SAI_xCR2_MUTECNT, 0) |
                     rv(SAI_xCR2_MUTEVAL, 0) |
                     rv(SAI_xCR2_MUTE, 0) | // Don't mute
                     rv(SAI_xCR2_TRIS, 0) | // Don't tri-state the data lines
                     rv(SAI_xCR2_FFLUSH, 0) | // Don't
                     rv(SAI_xCR2_FTH, 2); // FIFO threshold at 1/2. Might not use...

    out_block->FRCR = rv(SAI_xFRCR_FSOFF, 0) | // Assert fs on bit 0
                      rv(SAI_xFRCR_FSPOL, 1) | // Rise on frame start
                      rv(SAI_xFRCR_FSDEF, 1) | // fs is sync and channel id (stereo)
                      rv(SAI_xFRCR_FSALL, 23) | // fs is up for 24 bit clocks
                      rv(SAI_xFRCR_FRL, 47); // Overall frame is 48 bit clocks
    out_block->SLOTR = rv(SAI_xSLOTR_SLOTEN, 0b11) | // Enable first 2 channels
                       rv(SAI_xSLOTR_NBSLOT, 1) | // Enable a total of 2 channels
                       rv(SAI_xSLOTR_SLOTSZ, 0) | // Slot size is data size
                       rv(SAI_xSLOTR_FBOFF, 0); // No extra bits at start

    out_block->IMR = 0; // No interrupts



    // Now enable the things
    in_block->CR1 |= rv(SAI_xCR1_SAIEN, 1);
    out_block->CR1 |= rv(SAI_xCR1_SAIEN, 1);
}

void AK4621::start(){
    dmaStreamSetMemory0(STM32_DMA_STREAM(dma_stream_rx), buffer_in);
    dmaStreamSetTransactionSize(STM32_DMA_STREAM(dma_stream_rx), in_buffer_size * 2);
    dmaStreamSetMode(STM32_DMA_STREAM(dma_stream_rx), dma_mode_rx);

    dmaStreamSetMemory0(STM32_DMA_STREAM(dma_stream_tx), buffer_out);
    dmaStreamSetTransactionSize(STM32_DMA_STREAM(dma_stream_rx), out_buffer_size * 2);
    dmaStreamSetMode(STM32_DMA_STREAM(dma_stream_tx), dma_mode_tx);

    if(cb_out){
        cb_out(buffer_out, out_buffer_size);
    }

    dmaStreamEnable(STM32_DMA_STREAM(dma_stream_rx));
    dmaStreamEnable(STM32_DMA_STREAM(dma_stream_tx));

    if(cb_out){
        cb_out(&buffer_out[out_buffer_size], out_buffer_size);
    }
}

void AK4621::stop(){
    dmaStreamDisable(STM32_DMA_STREAM(dma_stream_rx));
    dmaStreamDisable(STM32_DMA_STREAM(dma_stream_tx));
}

void AK4621::data_in_handler(void * device, uint32_t flags){
    AK4621 * codec = reinterpret_cast<AK4621 *>(device);
    sample_t const * src = codec->rx_buffer_sel ? codec->buffer_in : &codec->buffer_in[in_buffer_size];
    if(codec->cb_in){
        codec->cb_in(src, in_buffer_size, 1);
    }
    codec->rx_buffer_sel ^= 1;
}

void AK4621::data_out_handler(void * device, uint32_t flags){
    AK4621 * codec = reinterpret_cast<AK4621 *>(device);
    sample_t * dst = codec->tx_buffer_sel ? codec->buffer_out : &codec->buffer_out[out_buffer_size];
    if(codec->cb_out){
        codec->cb_out(dst, out_buffer_size);
    }
    codec->tx_buffer_sel ^= 1;
}

static volatile uint32_t dma_rx_isr_count = 0;
static volatile uint32_t dma_tx_isr_count = 0;

