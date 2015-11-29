/*!
 (C) Copyright 2013, Ben Nahill

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

 @file main.cpp
 @brief Application entry point. Mostly glue logic.
 @author Ben Nahill <bnahill@gmail.com>
 */

#include <stdint.h>
#include <derivative.h>
#include <driver/platform.h>
#include <controller.h>
#include <input_dsp.h>

extern "C" {
	void main();
}

void swo_sendchar(char x)
{
     if (
          (CoreDebug->DHCSR & CoreDebug_DHCSR_C_DEBUGEN_Msk) &&    // if debugger is attached (does it really work?)
          (CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)  &&      // Trace enabled
          (ITM->TCR & ITM_TCR_ITMENA_Msk)                  &&      // ITM enabled
          (ITM->TER & (1UL << 0))                                  // ITM Port #0 enabled
     )
     {
          volatile uint32_t i = 0xFFF; // implement a timeout
          while (ITM->PORT[0].u32 == 0 && i--); // wait for next
          if (ITM->PORT[0].u32 != 0 && i != 0) {
               ITM->PORT[0].u8 = (uint8_t) x; // send
          }
     }
}

void swo_setup(){
	uint32_t SWOSpeed = 6000000; //6000kbps, default for JLinkSWOViewer
	uint32_t SWOPrescaler = (120000000 / SWOSpeed) - 1; // SWOSpeed in Hz, note that F_CPU is expected to be 96000000 in this case
	CoreDebug->DEMCR = CoreDebug_DEMCR_TRCENA_Msk;
	*((volatile unsigned *)(ITM_BASE + 0x400F0)) = 0x00000002; // "Selected PIN Protocol Register": Select which protocol to use for trace output (2: SWO)
	*((volatile unsigned *)(ITM_BASE + 0x40010)) = SWOPrescaler; // "Async Clock Prescaler Register". Scale the baud rate of the asynchronous output
	*((volatile unsigned *)(ITM_BASE + 0x00FB0)) = 0xC5ACCE55; // ITM Lock Access Register, C5ACCE55 enables more write access to Control Register 0xE00 :: 0xFFC
	ITM->TCR = ITM_TCR_TraceBusID_Msk | ITM_TCR_SWOENA_Msk | ITM_TCR_SYNCENA_Msk | ITM_TCR_ITMENA_Msk; // ITM Trace Control Register
	ITM->TPR = ITM_TPR_PRIVMASK_Msk; // ITM Trace Privilege Register
	ITM->TER = 0x00000001; // ITM Trace Enable Register. Enabled tracing on stimulus ports. One bit per stimulus port.
	*((volatile unsigned *)(ITM_BASE + 0x01000)) = 0x400003FE; // DWT_CTRL
	*((volatile unsigned *)(ITM_BASE + 0x40304)) = 0x00000100; // Formatter and Flush Control Register
}

static SPI_slave volatile	test_slave = SPI_slave(
	Platform::spi0,
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
//#undef AUDIO_ENDPOINT_SIZE
//#define AUDIO_ENDPOINT_SIZE 64
static __attribute__((aligned(512))) float audio_test[USB::audioMaxSamples];
/*!
 * @brief Application entry point
 */
void main(){
	struct pt pt_dsp, pt_controller, pt_wavegen;
	
	PT_INIT(&pt_dsp);
	PT_INIT(&pt_controller);
	PT_INIT(&pt_wavegen);

	// Perform late initializations
	Platform::lateInit();

	USB::hidClassInit();

    Platform::codec.init();

    /*
    Platform::spi0.register_slave(flash_slave);

    uint8_t tx_buf[8] = {0x9f,0x00, 0, 0};
    uint8_t rx_buf[8] = {0};

    Platform::spi0.transfer(flash_slave, tx_buf, rx_buf, 4, nullptr, nullptr);
    */

// 	swo_setup();
// 	swo_sendchar('a');
// 	swo_sendchar('b');
// 	swo_sendchar('c');
// 	swo_sendchar('d');

	uint32_t i = 0;
    while(true){
		//InputDSP::pt_dsp(&pt_dsp);
		//WaveGen::pt_wavegen(&pt_wavegen);
		//APulseController::pt_controller(&pt_controller);
		if(USB::audioReady() and !USB::audioQueueFull()){
			for(int j = 0; j < USB::audioMaxSamples; j++){
				audio_test[j] = (i+j) * 0.001f;
			}
			USB::audioSend(audio_test, USB::audioMaxSamples);
			i += USB::audioMaxSamples;
			if(i > (1000 - USB::audioMaxSamples))
				i -= (1000 - USB::audioMaxSamples);
		}
	}
}

