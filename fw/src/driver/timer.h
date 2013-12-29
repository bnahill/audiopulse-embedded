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
 @brief A simplified interface to the PIT peripherals
 
 @note This totally ignores most of the features of PIT, at least for now
 */
class Timer {
public:
	constexpr Timer(FTM_MemMapPtr FTM):
		FTM(FTM)
	{}
	
	typedef enum {
		PS_1   = 0,
		PS_2   = 1,
		PS_4   = 2,
		PS_8   = 3,
		PS_16  = 4,
		PS_32  = 5,
		PS_64  = 6,
		PS_128 = 7
	} prescaler_t;
	
	typedef enum {
		CLKS_NONE  = 0,
		CLKS_SYS   = 1,
		CLKS_FIXED = 2,
		CLKS_EXT   = 3
	} clock_src_t;
	
	void configure(clock_src_t cs, prescaler_t ps, uint16_t modulo) const {
		if(FTM == FTM0_BASE_PTR)
			SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;
		else if(FTM == FTM1_BASE_PTR)
			SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;
		else if(FTM == FTM2_BASE_PTR)
			SIM_SCGC3 |= SIM_SCGC3_FTM2_MASK;
		// Disable device, allow for configuration
		stop();
		FTM->SC = FTM_SC_PS(ps) | FTM_SC_CLKS(cs);
	}
	
	void start() const {
		// Don't care about other fields
		FTM->MODE = FTM_MODE_FTMEN_MASK;
	}
	
	void stop() const {
		// Don't care about other fields
		FTM->MODE = FTM_MODE_WPDIS_MASK;
	}
	
	void reset_count(uint16_t initial) const {
		FTM->CNTIN = FTM_CNTIN_INIT(initial);
		// Force reload
		FTM->CNT = 0;
	}
	
	uint16_t get_count() const {
		return FTM->CNT;
	}
	
	bool did_overflow() const {
		uint32_t v = FTM->SC;
		if(v & FTM_SC_TOF_MASK){
			FTM->SC = v & ~FTM_SC_TOF_MASK;
			return true;
		}
		return false;
	}
	
protected:
	FTM_MemMapPtr const FTM;
};

#endif // __APULSE_TIMER_H_
