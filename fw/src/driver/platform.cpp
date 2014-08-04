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

PWMFTM const Platform::pwm_ftm = {FTM0_BASE_PTR, GPIOPin::MUX_ALT4};

PWMGPIOPin const Platform::pwm[] = {
	PWMGPIOPin(Platform::pwm_ftm, Platform::leds[0], 4),
	PWMGPIOPin(Platform::pwm_ftm, Platform::leds[1], 5),
	PWMGPIOPin(Platform::pwm_ftm, Platform::leds[2], 3)
};

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

	power_en.clear();
	power_en.make_output();
	power_en.configure(GPIOPin::MUX_GPIO, true);


	for(auto &led : leds){
		// Setup that one LED so it can be used for debugging
		led.make_output();
		led.clear();
		led.configure(GPIOPin::MUX_GPIO, true);
	}

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

