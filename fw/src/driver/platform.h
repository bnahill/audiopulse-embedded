#ifndef __APULSE_PLATFORM_H_
#define __APULSE_PLATFORM_H_

#ifdef __cplusplus

#include <driver/clocks.h>
#include <driver/usb.h>
#include <driver/gpio.h>
#include <driver/codec.h>
#include <pt.h>

class Platform {
public:
	/*!
	 * @brief Initialization to be done before anything
	 *
	 * @note Since this may be done before data initialization, it may only use
	 * constants
	 */
	static void earlyInit();

	/*!
	 @brief Initialize peripherals and devices that depend on clocks and data initialization
	 */
	static void lateInit(){

		USB::lateInit();

		AK4621::init();
		
		led.set();
	}
	
	//! The one debug LED
	static constexpr GPIOPin led = {PTB_BASE_PTR, 19};
};

#endif // __cplusplus


#ifdef __cplusplus
extern "C" {
#endif
/*!
 * @brief An earlyInit with C-ready linkage
 */
void earlyInitC();
#ifdef __cplusplus
};
#endif

#endif // __APULSE_PLATFORM_H_

