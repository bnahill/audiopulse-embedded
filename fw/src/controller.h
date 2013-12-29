/*!
 (C) Copyright 2013, Ben Nahill
 
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

#ifndef __APULSE_CONTROLLER_H_
#define __APULSE_CONTROLLER_H_

#include <driver/platform.h>
#include <driver/codec.h>
#include <driver/timer.h>


class APulseController {
	typedef enum {
		CMD_RESET = 0,   // Reset the controller
		CMD_STARTUP = 1, // Enable CODEC and power-hungry crap
		CMD_SETUPTONES = 2,
		CMD_SETUPCAPTURE = 3,
		CMD_GETSTATUS = 4,
		CMD_GETDATA = 5,
		CMD_START = 6,
	} cmd_t;

	/*!
	 @brief A mode for a single tone

	 This is intended to be or'd with a channel number
	 */
	typedef enum {
		TONE_OFF = 0   << 4,
		TONE_FIXED = 1 << 4,
		TONE_CHIRP = 2 << 4,
	} tone_mode_t;

	/*!
	 @brief The configuration for a single tone
	 */
	typedef struct {
		//! of type tone_mode_t or'd with a channel number
		uint8_t tone_mode_ch;
		//! The SPL, in dB 16-bit Q8
		uint16_t amplitude;
		//! Frequency in Hz
		uint16_t f1;
		//! Frequency 2 in Hz (not used for fixed tones)
		uint16_t f2;
		//! Duration in ms -- set to 0 for no limit
		uint16_t t;
	} __attribute__((packed)) tone_config_t;

	typedef struct {
		uint8_t cmd;
		uint8_t enabled_mask;
		tone_config_t tones[3];
	} __attribute__((packed)) tone_setup_pkt_t;

	typedef struct {
		uint8_t cmt;
		//! Window overlap in Q8
		uint8_t overlap;
		//! Window function (ignored)
		uint8_t window_function;
		//! Number of windows to capture
		uint16_t num_windows;
	} __attribute__((packed)) poo_t;

public:

	/*!
	 @brief Handle a USB command (or data sent)
	 */
	static void handle_data(uint8_t * data, uint8_t size){

	}

	/*!
	 @brief Get a response

	 This response should already be prepared
	 */
	static uint8_t * get_response(uint16_t &size){
		size = 0;
		return nullptr;
	}
	
	static uint16_t get_time_ms(){
		return timer.get_count();
	}


	static PT_THREAD(pt_command_parser(struct pt * pt)){
		PT_BEGIN(pt);

		while(true){
			PT_YIELD(pt);
		}

		PT_END(pt);
	}

	static PT_THREAD(pt_wavegen(struct pt * pt)){
		PT_BEGIN(pt);
		
		// Run from 32kHz/32 clock
		timer.configure(Timer::CLKS_FIXED, Timer::PS_32, 0);
		timer.reset_count(0);

		while(true){
			PT_YIELD(pt);
		}

		PT_END(pt);
	}
	
	static constexpr Timer timer = FTM0_BASE_PTR;
};

#endif // __APULSE_CONTROLLER_H_
