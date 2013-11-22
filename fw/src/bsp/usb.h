#ifndef __APULSE_USB_H_
#define __APULSE_USB_H_

#include <derivative.h>

namespace K20 {

class USB {
public:
	static void lateInit(){
		// Divide 48MHz by 1 for USB 48MHz
		SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(0);

		// Configure USB to be clocked from PLL
		SIM_SOPT2 |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL_MASK;

		// Enable USB-OTG IP clocking
		SIM_SCGC4 |= SIM_SCGC4_USBOTG_MASK;

		// old documentation writes setting this bit is mandatory
		USB0_USBTRC0 = 0x40;
		
		// Configure enable USB regulator for device
		SIM_SOPT1 |= SIM_SOPT1_USBREGEN_MASK;

		NVICICPR2 = (1 << 9);	// Clear any pending interrupts on USB
		NVICISER2 = (1 << 9);	// Enable interrupts from USB module
	}
	
	static void audioClassInit();
	static void audioSend();
private:
	static void callback(uint8_t controller_ID,
	                     uint8_t event_type, void * val);
	static void prepare_data();
};

}

#endif // __APULSE_USB_H_
