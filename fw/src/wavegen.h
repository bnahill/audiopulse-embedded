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
#ifndef __APULSE_WAVEGEN_H_
#define __APULSE_WAVEGEN_H_

#include <driver/platform.h>
#include <driver/codec.h>

class WaveGen {
public:
	typedef AK4621::sample_t sample_t;
	
	/*!
	 * 
	 * @brief A callback to get samples
	 * @param dst The destination buffer (size known)
	 * 
	 * @note This should be called from interrupt or locked context
	 * 
	 * @note Generation will be performed immediately from this context due to
	 * short deadline
	 * 
	 * @note Inlined because only a single caller
	 */
	static void get_samplesI(sample_t * dst){
		// Zero the whole damn thing first
		zero4(dst, buffer_size);
		// Check if muting
		if(silent)
			return;
		
		for(auto &generator : generators){
			sample_t * iter = dst;
			switch(generator.type){
			case GEN_OFF:
				// Don't care
				continue;
			case GEN_FIXED:
				// Go add a fixed wave to the appropriate channels
				break;
			case GEN_CHIRP:
				break;
			}
		}
	}
	
	static void zero4(sample_t * dst, uint32_t n){
		register uint32_t loops = (n + 15) >> 4;
		if(!n) return; // But why would anyone do this?
		switch(n & 0xFF){
		case 0: do { *dst++ = 0;
		case 15: *dst++ = 0;
		case 14: *dst++ = 0;
		case 13: *dst++ = 0;
		case 12: *dst++ = 0;
		case 11: *dst++ = 0;
		case 10: *dst++ = 0;
		case 9: *dst++ = 0;
		case 8: *dst++ = 0;
		case 7: *dst++ = 0;
		case 6: *dst++ = 0;
		case 5: *dst++ = 0;
		case 4: *dst++ = 0;
		case 3: *dst++ = 0;
		case 2: *dst++ = 0;
		case 1: *dst++ = 0;
			} while(--loops > 0);
		}
	}
	
	//! @name Muting functions
	//! Mute all channels when altering parameters
	//! @{
	static void mute(){
		silent = true;
	}
	static void unmute(){
		silent = false;
	}
	//! @}
protected:
	static constexpr uint32_t buffer_size = AK4621::out_buffer_size;
	static constexpr uint32_t wavetable_len = 4096;
	static const sample_t wavetable[wavetable_len];
	
	static bool silent;
	
	typedef enum {
		GEN_OFF = 0,
		GEN_FIXED,
		GEN_CHIRP
	} gen_type_t;
	
	static constexpr uint32_t num_generators = 3;
	typedef struct {
		gen_type_t type;
		uint16_t f1;     //!< Start frequency
		uint16_t f2;     //!< End frequency (chirp only)
		uint16_t t1;     //!< The start time
		uint16_t t2;     //!< The finish time
		uint16_t theta;  //!< Current phase in wavetable
		uint16_t f_current; //!< Theta for frequency in chirp
		uint8_t ch;
		sample_t gain;   //!< Gain applied to each full-scale sample
	} generator_t;
	
	static generator_t generators[num_generators];
};

#endif // __APULSE_WAVEGEN_H_


