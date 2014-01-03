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
#include <driver/tpa6130a2.h>
#include <wavegen.h>
#include "input_dsp.h"


class APulseController {
	//! @name Commands
	//! @{

	typedef enum {
		CMD_RESET = 0,   // Reset the controller
		CMD_STARTUP = 1, // Enable CODEC and power-hungry crap
		CMD_SETUPTONES = 2,
		CMD_SETUPCAPTURE = 3,
		CMD_GETSTATUS = 4,
		CMD_GETDATA = 5,
		CMD_START = 6,

		CMD_NONE = 255,
	} cmd_t;

	/*!
	 @brief A mode for a single tone

	 This is intended to be or'd with a channel number
	 */
	typedef enum {
		TONE_OFF   = 0,
		TONE_FIXED = 1,
		TONE_CHIRP = 2,
	} tone_mode_t;

	/*!
	 @brief The configuration for a single tone
	 */
	typedef struct {
		//! tone_mode_t mode
		uint8_t mode : 4;
		//! Channel number
		uint8_t ch   : 4;
		//! The SPL, in dB
		sFractional<7,8> amplitude;
		//! Frequency in Hz
		uint16_t f1;
		//! Frequency 2 in Hz (not used for fixed tones)
		uint16_t f2;
		//! Start time in ms
		uint16_t t1;
		//! End time in ms
		uint16_t t2;
	} __attribute__((packed)) tone_config_t;

	typedef struct {
		uint8_t cmd;
		uint8_t enabled_mask;
		tone_config_t tones[3];
	} __attribute__((packed)) tone_setup_pkt_t;

	typedef struct {
		uint8_t cmd;
		//! Window overlap in samples
		uint8_t overlap;
		//! Window function (ignored)
		uint8_t window_function;
		//! Number of windows to capture
		uint16_t num_windows;
		//! The time in ms to start capturing
		uint16_t start_time;
	} __attribute__((packed)) capture_config_pkt_t;

	typedef union {
		capture_config_pkt_t capture_config_pkt;
		tone_setup_pkt_t     tone_config_pkt;
	} r_packet_t;

	//! @}


	//! @name Return messages
	//! @{

	typedef struct {
		uint8_t version;
		uint8_t is_capturing     : 1;
		uint8_t is_playing       : 1;
		uint8_t is_started       : 1;
		uint8_t reset_wavegen    : 1;
		uint8_t reset_input      : 1;
		uint8_t reset_controller : 1;
	} status_pkt_t;

	typedef union {
		status_pkt_t status_pkt;
	} t_packet_t;

	//! @}

	typedef enum {
		ST_RESET = 0,
		ST_GETPSD,
		ST_GETAVG,
		ST_RESETTING,
	} state_t;
public:

	/*!
	 @brief Handle a USB command (or data sent)
	 @param data A pointer tor the received data
	 @param size The number of bytes in the packet

	 @note THIS SHOULD BE CALLED FROM INTERRUPT OR LOCKED CONTEXT
	 */
	static void handle_dataI(uint8_t * data, uint8_t size){
		auto const a = reinterpret_cast<r_packet_t *>(data);
		(void)a;
		// First make sure there is data to parse
		if(!size)
			return;

		static_assert(sizeof(tone_config_t) == 11,
		              "tone_config_t wrong size");
		static_assert(sizeof(tone_setup_pkt_t) == 3*sizeof(tone_config_t) + 2,
		              "tone_setup_pkt_t wrong size");
		static_assert(sizeof(status_pkt_t) == 2,
		              "status_pkt_t wrong size");
		static_assert(sizeof(capture_config_pkt_t) == 7,
		              "capture_config_pkt wrong size");

		switch(*data){
		case CMD_RESET:
			timer.stop();
			timer.reset_count();

			WaveGen::request_resetI();
			InputDSP::request_resetI();
			break;
		case CMD_GETDATA:
			state = ST_GETPSD;
			cmd_idx = 0;
			break;
		case CMD_GETSTATUS:
			// Does this really need to happen?
			break;
		case CMD_SETUPCAPTURE:
			InputDSP::configure(a->capture_config_pkt.overlap,
			                    a->capture_config_pkt.start_time,
			                    a->capture_config_pkt.num_windows);
			break;
		case CMD_SETUPTONES:
			WaveGen::mute();
			for(uint32_t i = 0; i < 3; i++){
				switch(a->tone_config_pkt.tones[i].mode){
				case TONE_FIXED:
					WaveGen::set_tone(i,
						a->tone_config_pkt.tones[i].ch,
						a->tone_config_pkt.tones[i].f1,
						a->tone_config_pkt.tones[i].t1,
						a->tone_config_pkt.tones[i].t2,
						a->tone_config_pkt.tones[i].amplitude);
					break;
				case TONE_CHIRP:
					WaveGen::set_off(i);
					break;
				case TONE_OFF:
				default:
					WaveGen::set_off(i);
					break;
				}
			}
			break;
		case CMD_STARTUP:
			break;
		case CMD_START:
			timer.stop();
			timer.reset_count();
			WaveGen::unmute();
			TPA6130A2::enable();
			timer.start();
			break;
		default:
			return;
		}
// 		if(size == 1 and data[0] == 0xA5){
// 			WaveGen::mute();
// 			WaveGen::set_tone(0, 0, 800, 1000, 10000, 65.0);
// 			WaveGen::set_tone(1, 1, 1600, 1000, 10000, 65.0);
// 			WaveGen::unmute();
// 			TPA6130A2::enable();
// 		} else if (size == 1 and data[0] == 0xAA){
// 			TPA6130A2::disable();
// 			WaveGen::mute();
// 			WaveGen::set_off(0);
// 			WaveGen::set_off(1);
// 		}
	}


	/*!
	 @brief Get a response

	 This response should already be prepared
	 */
	static uint8_t * get_response(uint16_t &size){
		static union {
			uint8_t data[64];
			status_pkt_t status;
			uint32_t data32[8];
		} p;

		switch(state){
		case ST_GETPSD:
			if(cmd_idx == 16){
				// Just one more value!
				((InputDSP::powerFractional *)&p)[0] = InputDSP::get_psd()[InputDSP::transform_len / 2];
				state = ST_GETAVG;
				cmd_idx = 0;
				size = 4;
				return p.data;
			}
			arm_copy_q31((q31_t*)p.data, (q31_t*)&InputDSP::get_psd()[cmd_idx++ * 8], 8);
			size = 64;
			return p.data;
		case ST_GETAVG:
			arm_copy_q31((q31_t*)p.data, (q31_t*)&InputDSP::get_average()[cmd_idx++ * 8], 8);
			if(cmd_idx == 16){
				state = ST_RESET;
				cmd_idx = 0;
			}
			size = 64;
			return p.data;
		case ST_RESET:
		case ST_RESETTING:
		default:
			zero16(p.data, sizeof(status_pkt_t));
			p.status.version = 0;
			p.status.is_started = 1;
			p.status.is_capturing = 1;
			p.status.is_playing = 1;

			p.status.reset_controller = 0;
			p.status.reset_input = InputDSP::is_resetI() ? 1 : 0;
			p.status.reset_wavegen = WaveGen::is_resetI() ? 1 : 0;
			size = sizeof(status_pkt_t);
			return p.data;
		}
		return nullptr;
	}
	
	static uint16_t get_time_ms(){
		return timer.get_count();
	}

	void request_resetI();


	static PT_THREAD(pt_command_parser(struct pt * pt)){
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
private:
	static state_t state;
	static uint32_t cmd_idx;
};

#endif // __APULSE_CONTROLLER_H_
