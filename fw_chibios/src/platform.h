#ifndef __APULSE_PLATFORM_H
#define __APULSE_PLATFORM_H

#include <ch.h>
#include <hal.h>

#include <codec.h>
#include <usbserial.h>

class Platform {
public:
    Platform();

    static constexpr GPIOPin pin_pwr = {_GPIOA, GPIOA_ANALOG_EN};
    static constexpr GPIOPin pin_clk = {_GPIOA, GPIOA_CLK_EN};

    inline static AK4621 codec = {SAI2, SAI2_Block_B, SAI1, SAI1_Block_A, SPID3,
                                  {_GPIOD, GPIOD_SPI_CS_CODEC}, {_GPIOB, GPIOB_CODEC_PDN}, {_GPIOA, GPIOA_CLK_EN},
                                  STM32_SAI1A_DMA_CHN, STM32_SAI_SAI1A_DMA_STREAM, STM32_SAI_SAI2B_DMA_PRIORITY, STM32_SAI_SAI1A_DMA_IRQ_PRIORITY,
                                  STM32_SAI2B_DMA_CHN, STM32_SAI_SAI2B_DMA_STREAM, STM32_SAI_SAI1A_DMA_PRIORITY, STM32_SAI_SAI2B_DMA_IRQ_PRIORITY};


};

#endif // __APULSE_PLATFORM_H
