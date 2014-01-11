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
#ifndef __APULSE_TIMER_H_
#define __APULSE_TIMER_H_

#include <driver/platform.h>

/*!
 @brief A quick interface to the PIT timers

 This doesn't deal with interrupts or any more advanced features...
 */
class TimerPIT {
public:
	constexpr TimerPIT(uint32_t index) : i(index) {}
	
	void reset() const {
		SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;

		PIT_MCR = PIT_MCR_FRZ_MASK; // Enable timer clocks but stop in debug
		PIT_TCTRL(i) = 0; // Disable timer
		PIT_LDVAL(i) = 0xFFFFFFFF;
	}
	
	void start() const {
		PIT_TCTRL(i) = PIT_TCTRL_TEN_MASK;
	}
	
	bool is_running() const {
		return PIT_TCTRL(i) & PIT_TCTRL_TEN_MASK;
	}
	
	uint32_t get_ms() const {
		return to_ms(PIT_CVAL(i));
	}
protected:
	static constexpr uint32_t to_ms(uint32_t cval){
		return ((uint64_t)(0xFFFFFFFF-cval) * mul) >> 32;
	}
	
	static constexpr uint32_t div = (Clock::config.busclk / 1000);
	static constexpr uint32_t mul = (uint32_t)((((uint64_t)(1)) << 32) / div);
	
	uint32_t const i;
};


#endif // __APULSE_TIMER_H_
