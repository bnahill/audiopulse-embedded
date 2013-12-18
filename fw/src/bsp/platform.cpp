#include <bsp/platform.h>

void earlyInitC(){
	WDOG_UNLOCK = 0xC520;
	WDOG_UNLOCK = 0xD928;
	WDOG_STCTRLH = 0x4010;
	Platform::earlyInit();
}

void Platform::earlyInit(){
	// Enable all of the GPIO ports
	SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
			  | SIM_SCGC5_PORTB_MASK
			  | SIM_SCGC5_PORTC_MASK
			  | SIM_SCGC5_PORTD_MASK
			  | SIM_SCGC5_PORTE_MASK);

	// Setup that one LED so it can be used for debugging
	led.make_output();
	led.clear();
	led.configure(GPIOPin::MUX_GPIO, true);

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

