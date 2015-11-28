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

#ifndef __APULSE_USB_H_
#define __APULSE_USB_H_

#include <derivative.h>
#include <usb_class.h>
#include <driver/clocks.h>
#include <usb_composite.h>
#include <usb_audio.h>

class USB {
public:
	static void lateInit(){
		static_assert(Clock::config.mcgoutclk == 48000000 or
		              Clock::config.mcgoutclk == 72000000 or
		              Clock::config.mcgoutclk == 120000000, "Invalid busclk value");
		switch(Clock::config.mcgoutclk){
		case 48000000:
			// Divide 48MHz by 1 for USB 48MHz
			SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(0);
			break;
		case 72000000:
			// Multiply 72MHz by 2 and then divide by 3
			SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(2) | SIM_CLKDIV2_USBFRAC_MASK;
			break;
		case 120000000:
			// Multiply 120MHz by 2 and then divide by 5
			SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(4) | SIM_CLKDIV2_USBFRAC_MASK;
			break;
		default:
			break;
		}

		// Enable USB-OTG IP clocking
		SIM_SCGC4 |= SIM_SCGC4_USBOTG_MASK;
		
		// Configure USB to be clocked from PLL
		SIM_SOPT2 |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(1);
		
		USB0_CLK_RECOVER_IRC_EN = 0;
		

		// old documentation writes setting this bit is mandatory
		USB0_USBTRC0 = 0x40;

		// Configure enable USB regulator for device
		// SIM_SOPT1 |= SIM_SOPT1_USBREGEN_MASK;

		// ICPR[2] & (1 << 9) for K20
		NVIC->ICPR[1] = (1 << 21);	// Clear any pending interrupts on USB
		NVIC->ISER[1] = (1 << 21);	// Enable interrupts from USB module
	}

	static void hidClassInit();
	static void audioClassInit();
	static void audioSend(uint8_t * buffer, uint16_t n);
	
	static bool audioReady(){return audio_ready;}
	static bool audioEmpty(){return audio_empty;}
	static bool hidReady(){return hid_ready;}
	
private:
	static bool audio_ready;
	static bool hid_ready;
	static bool audio_empty;
	
	static void HIDcallback(uint8_t controller_ID,
	                        uint8_t event_type, void * val);
	static uint8_t HIDParamCallback(uint_8 request,
	                                uint_16 value,
	                                uint_16 wIndex,
	                                uint8_t ** data,
	                                USB_PACKET_SIZE * size);
	static void AudioCallback(uint8_t controller_ID,
	                          uint8_t event_type, void * val);
	static uint8_t AudioParamCallback(uint_8 request,
	                                  uint_16 value,
	                                  uint_16 wIndex,
	                                  uint8_t ** data,
	                                  USB_PACKET_SIZE * size);
	
	
	
	static CLASS_APP_CALLBACK_STRUCT hid_class_callback_struct;
	static CLASS_APP_CALLBACK_STRUCT audio_class_callback_struct;
	static COMPOSITE_CALLBACK_STRUCT usb_composite_callback;

	/*!
	 * \brief callback_param
	 * \param request [IN] request type
	 * \param value [IN] report type and ID
	 * \param data [OUT] pointer to the data
	 * \param size [OUT] size of the transfer
	 * \return
	 */
	static uint8_t callback_param(uint8_t request, uint16_t value,
								  uint16_t iface, uint8_t ** data,
								  USB_PACKET_SIZE * size);

	static void prepare_data();

	static constexpr uint8_t CONTROLLER_ID = 0;
};


#endif // __APULSE_USB_H_
