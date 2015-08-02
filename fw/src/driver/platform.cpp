// vim: noai:ts=4:sw=4
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

#include <driver/platform.h>

GPIOPin const Platform::leds[] = {{PTD_BASE_PTR, 4}, {PTD_BASE_PTR, 5}, {PTC_BASE_PTR, 4}};

GPIOPin const Platform::power_en = {PTD_BASE_PTR, 3};
GPIOPin const Platform::uart_rx = {PTD_BASE_PTR, 6};
GPIOPin const Platform::uart_tx = {PTD_BASE_PTR, 7};

PWMFTM const Platform::pwm_ftm = {FTM0_BASE_PTR, GPIOPin::MUX_ALT4};

PWMGPIOPin const Platform::pwm[] = {
	PWMGPIOPin(Platform::pwm_ftm, Platform::leds[0], 4),
	PWMGPIOPin(Platform::pwm_ftm, Platform::leds[1], 5),
	PWMGPIOPin(Platform::pwm_ftm, Platform::leds[2], 3)
};

SPI Platform::spi0 = {
	SPI0_BASE_PTR,
	{PTD_BASE_PTR, 2}, GPIOPin::MUX_ALT2,
	{PTC_BASE_PTR, 7}, GPIOPin::MUX_ALT2,
	{PTD_BASE_PTR, 1}, GPIOPin::MUX_ALT2,
	2, 3 // DMA channels 2 and 3 for TX and RX
};

AK4621 Platform::codec = {
    {PTC_BASE_PTR, 6}, GPIOPin::MUX_ALT6, // MCLK
    {PTC_BASE_PTR, 2}, GPIOPin::MUX_ALT6, // LRCK
    {PTC_BASE_PTR, 3}, GPIOPin::MUX_ALT6, // BCLK
    {PTC_BASE_PTR, 1}, GPIOPin::MUX_ALT6, // SDO
    {PTC_BASE_PTR, 5}, GPIOPin::MUX_ALT4, // SDIN
    {PTB_BASE_PTR, 18}, // XTAL
    {PTB_BASE_PTR, 3},  //PDN
    I2S0_BASE_PTR,
    1, // RX DMA Channel
    0, // TX DMA Channel
    SPI_slave( // SPI slave
        spi0,
        {PTD_BASE_PTR, 0}, GPIOPin::MUX_ALT2,
        (
            SPI_CTAR_FMSZ(7)   |   // 8-bit frames
            SPI_CTAR_CPOL_MASK |   // Clock idle high
            SPI_CTAR_CPHA_MASK |   // Sample following edge
            SPI_CTAR_PCSSCK(2) |   // PCS to SCK prescaler = 5
            SPI_CTAR_PASC(2)   |   // Same delay before end of PCS
            SPI_CTAR_PDT(3)    |   // Same delay after end of PCS
            SPI_CTAR_PBR(0)    |   // Use sysclk / 2
            SPI_CTAR_CSSCK(6)  |   // Base delay is 128*Tsys
            SPI_CTAR_ASC(3)    |   // Unit delay before end of PCS
            SPI_CTAR_DT(3)     |   // Same after end of PCS
            SPI_CTAR_BR(8)         // Scale by 256
        ),
        0
    )
};

SPI_slave Platform::codec_slave = SPI_slave(
	spi0,
	{PTD_BASE_PTR, 0}, GPIOPin::MUX_ALT2,
	(
		SPI_CTAR_FMSZ(7)   |   // 8-bit frames
		SPI_CTAR_CPOL_MASK |   // Clock idle high
		SPI_CTAR_CPHA_MASK |   // Sample following edge
		SPI_CTAR_PCSSCK(2) |   // PCS to SCK prescaler = 5
		SPI_CTAR_PASC(2)   |   // Same delay before end of PCS
		SPI_CTAR_PDT(3)    |   // Same delay after end of PCS
		SPI_CTAR_PBR(0)    |   // Use sysclk / 2
		SPI_CTAR_CSSCK(6)  |   // Base delay is 128*Tsys
		SPI_CTAR_ASC(3)    |   // Unit delay before end of PCS
		SPI_CTAR_DT(3)     |   // Same after end of PCS
		SPI_CTAR_BR(8)         // Scale by 256
	),
    0
);

SPI_slave Platform::flash_slave = SPI_slave(
    Platform::spi0,
    {PTC_BASE_PTR, 0}, GPIOPin::MUX_ALT2,
    (
        SPI_CTAR_FMSZ(7)   |   // 8-bit frames
        SPI_CTAR_CPOL_MASK |   // Clock idle high
        SPI_CTAR_CPHA_MASK |   // Sample following edge
        SPI_CTAR_PCSSCK(2) |   // PCS to SCK prescaler = 5
        SPI_CTAR_PASC(2)   |   // Same delay before end of PCS
        SPI_CTAR_PDT(3)    |   // Same delay after end of PCS
        SPI_CTAR_PBR(0)    |   // Use sysclk / 2
        SPI_CTAR_CSSCK(6)  |   // Base delay is 128*Tsys
        SPI_CTAR_ASC(3)    |   // Unit delay before end of PCS
        SPI_CTAR_DT(3)     |   // Same after end of PCS
        SPI_CTAR_BR(8)         // Scale by 256
    ),
    0
);

MT29FxG01 Platform::flash = MT29FxG01(
    Platform::spi0,
    MT29FxG01::SIZE_4G,
    Platform::flash_slave
);

void earlyInitC(){
#if ((__FPU_PRESENT == 1) && (__FPU_USED == 1))
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));    /* set CP10, CP11 Full Access */
#endif /* ((__FPU_PRESENT == 1) && (__FPU_USED == 1)) */

if((RCM->SRS0 & RCM_SRS0_WAKEUP_MASK) != 0x00U){
	if((PMC->REGSC & PMC_REGSC_ACKISO_MASK) != 0x00U)
	{
		PMC->REGSC |= PMC_REGSC_ACKISO_MASK; /* Release hold with ACKISO:  Only has an effect if recovering from VLLSx.*/
	}
}

#ifdef SYSTEM_SMC_PMPROT_VALUE
	SMC->PMPROT = SYSTEM_SMC_PMPROT_VALUE;
#endif
	
#if (((SYSTEM_SMC_PMCTRL_VALUE) & SMC_PMCTRL_RUNM_MASK) == (0x03U << SMC_PMCTRL_RUNM_SHIFT))
	SMC->PMCTRL = (uint8_t)((SYSTEM_SMC_PMCTRL_VALUE) & (SMC_PMCTRL_RUNM_MASK)); /* Enable HSRUN mode */
	while(SMC->PMSTAT != 0x80U) {}
#endif

	// WDOG->UNLOCK: WDOGUNLOCK=0xC520
	WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xC520); /* Key 1 */
	// WDOG->UNLOCK: WDOGUNLOCK=0xD928
	WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xD928); /* Key 2 */
	WDOG->STCTRLH = WDOG_STCTRLH_BYTESEL(0x00) |
	                WDOG_STCTRLH_WAITEN_MASK |
	                WDOG_STCTRLH_STOPEN_MASK |
	                WDOG_STCTRLH_ALLOWUPDATE_MASK |
	                WDOG_STCTRLH_CLKSRC_MASK |
	                0x0100U;
	Platform::earlyInit();
}

void Platform::earlyInit(){
	// Enable all of the GPIO ports
	SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
			  | SIM_SCGC5_PORTB_MASK
			  | SIM_SCGC5_PORTC_MASK
			  | SIM_SCGC5_PORTD_MASK
			  | SIM_SCGC5_PORTE_MASK);

    SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
    SIM_SCGC7 |= SIM_SCGC7_DMA_MASK;

    DMA_CR =
        DMA_CR_EMLM_MASK |      // Enable minor looping
        DMA_CR_ERCA_MASK;       // Enable RR arbitration

	power_en.clear();
	power_en.make_output();
    power_en.configure(GPIOPin::MUX_GPIO, true);

#if CFG_POWER_ALWAYS_ON
    power_on();
#endif

	for(auto &led : leds){
		// Setup that one LED so it can be used for debugging
		led.make_output();
		led.clear();
		led.configure(GPIOPin::MUX_GPIO, true);
	}

    uart_tx.make_output();
    uart_tx.clear();
    uart_tx.configure(GPIOPin::MUX_GPIO, true);

	Clock::setupClocks();
}

extern "C" {
extern uint8_t const cfmconf[16];
};


#if 1
__attribute__((section(".cfmconfig")))
decltype(cfmconf) cfmconf = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff
};
#endif

void SPI0_ISR(){
    Platform::spi0.handle_dready_isr();
}

void DMA_CH0_ISR(){
    Platform::codec.dma_tx_isr();
}

void DMA_CH1_ISR(){
    Platform::codec.dma_rx_isr();
}

void DMA_CH2_ISR(){
	Platform::spi0.handle_complete_isr();
}

void DMA_CH3_ISR(){
	Platform::spi0.handle_complete_isr();
}
