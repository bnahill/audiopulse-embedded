#ifndef __APULSE_PLATFORM_H_
#define __APULSE_PLATFORM_H_

#ifdef __cplusplus

#include <bsp/clocks.h>
#include <bsp/usb.h>
#include <bsp/gpio.h>
#include <bsp/codec.h>

using namespace K20;

class Platform {
public:
	/*!
	 * @brief Initialization to be done before anything
	 *
	 * @note Since this may be done before data initialization, it may only use
	 * constants
	 */
	static void earlyInit();

	static void lateInit(){

		USB::lateInit();

		AK4621::init();
	}
	
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

