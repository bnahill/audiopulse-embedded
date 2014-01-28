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
#include <input_dsp.h>

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
		CMD_CALIBRATE_MIC = 7,
		CMD_RESET_CALIB = 8,

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
		tone_config_t tones[3];
	} __attribute__((packed)) tone_setup_pkt_t;

	typedef struct {
		uint8_t cmd;
		//! Window overlap in samples
		uint16_t overlap;
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
		uint8_t input_state;
		uint8_t wavegen_state;
		uint8_t controller_state;
		uint8_t err_code;
	} status_pkt_t;

	typedef union {
		status_pkt_t status_pkt;
	} t_packet_t;

	//! @}

	typedef enum {
		TEST_RESET       = 0,
		TEST_CONFIGURING = 1,
		TEST_READY       = 2,
		TEST_RUNNING     = 3,
		TEST_DONE        = 4,
		TEST_CALIB_MIC   = 5
	} teststate_t;

	typedef enum {
		ST_RESET = 0,
		ST_GETPSD,
		ST_GETAVG,
		ST_RESETTING,
	} state_t;

	static void clear_calibration(){
		for(auto &i : coeffs) i = 1.0;
	}
public:
	static InputDSP::powerFractional coeffs[16];

	/*!
	 @brief Handle a USB command (or data sent)
	 @param data A pointer tor the received data
	 @param size The number of bytes in the packet

	 @note THIS SHOULD BE CALLED FROM INTERRUPT OR LOCKED CONTEXT
	 */
	static void handle_dataI(uint8_t * data, uint8_t size);


	/*!
	 @brief Get a response

	 This response should already be prepared
	 */
	static uint8_t * get_response(uint16_t &size);
	
	static uint16_t get_time_ms(){
		//return timer.get_count();
		uint32_t t = timer.get_ms();
		most_recent_t_ms = t;
		return t;
	}

	void request_resetI();

	static PT_THREAD(pt_controller(struct pt * pt));
	
	//! An instance of the timer for synchronization
	//static constexpr Timer timer = FTM0_BASE_PTR;
	static constexpr TimerPIT timer = {0};

	static constexpr sFractional<8,23> dbspl_reference = 90.0;
	//! dbspl_reference - 24 dB (/ 16)
	static constexpr sFractional<8,23> calib_tone_level = 66.0;
private:
	static uint32_t most_recent_t_ms;
	static teststate_t teststate;
	static state_t state;
	static uint32_t cmd_idx;
	static uint8_t err_code;
};

#endif // __APULSE_CONTROLLER_H_
