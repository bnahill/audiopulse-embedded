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

class USB {
public:
	static void lateInit(){
		static_assert(Clock::config.mcgoutclk == 48000000 or
					  Clock::config.mcgoutclk == 72000000, "Invalid busclk value");
		switch(Clock::config.mcgoutclk){
		case 48000000:
			// Divide 48MHz by 1 for USB 48MHz
			SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(0);
			break;
		case 72000000:
			// Multiply 72MHz by 2 and then divide by 3
			SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(2) | SIM_CLKDIV2_USBFRAC_MASK;
			break;
		default:
			break;
		}

		// Configure USB to be clocked from PLL
		SIM_SOPT2 |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL_MASK;

		// Enable USB-OTG IP clocking
		SIM_SCGC4 |= SIM_SCGC4_USBOTG_MASK;

		// old documentation writes setting this bit is mandatory
		USB0_USBTRC0 = 0x40;

		// Configure enable USB regulator for device
		// SIM_SOPT1 |= SIM_SOPT1_USBREGEN_MASK;

		NVIC->ICPR[2] = (1 << 9);	// Clear any pending interrupts on USB
		NVIC->ISER[2] = (1 << 9);	// Enable interrupts from USB module
	}

	static void hidClassInit();
	static void audioClassInit();
	static void audioSend();
private:
	static void callback(uint8_t controller_ID,
						 uint8_t event_type, void * val);

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
