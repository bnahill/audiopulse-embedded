#ifdef __cplusplus

#include <bsp/clocks.h>
#include <bsp/usb.h>

using namespace K20;

class Platform {
public:
	/*!
	 * @brief Initialization to be done before anything
	 * 
	 * @note Since this may be done before data initialization, it may only use
	 * constants
	 */
	static void earlyInit(){
		Clock::setupClocks();
	}
	
	static void lateInit(){
		// This shouldn't really be here but it is until later
		
		// Enable all of the GPIO ports
		SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
		          | SIM_SCGC5_PORTB_MASK
		          | SIM_SCGC5_PORTC_MASK
		          | SIM_SCGC5_PORTD_MASK
		          | SIM_SCGC5_PORTE_MASK);
		
		USB::lateInit();
	}
};

#endif


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
