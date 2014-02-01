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

#include <controller.h>

APulseController::state_t APulseController::state;
uint32_t APulseController::cmd_idx;
uint8_t APulseController::err_code;
decltype(APulseController::teststate) APulseController::teststate = TEST_RESET;
constexpr TimerPIT APulseController::timer;
InputDSP::powerFractional APulseController::coeffs[16];

uint32_t APulseController::most_recent_t_ms;

PT_THREAD(APulseController::pt_controller)(struct pt * pt){
	static uint32_t i;

	PT_BEGIN(pt);

	clear_calibration();

	timer.reset();
	
	// Run from 32kHz/32 clock
	//timer.configure(Timer::CLKS_FIXED, Timer::PS_32, 0);
	//timer.reset_count(0);

	while(true){
		PT_YIELD(pt);
		if(InputDSP::get_state() == InputDSP::ST_DONE &&
		   WaveGen::get_state() == WaveGen::ST_DONE){
			teststate = TEST_DONE;
			Platform::led.clear();
			if(timer.is_running())
				timer.reset();
		}
		if(teststate == TEST_CALIB_MIC){

			for(i = 0; i < 16; i++){
				// Calibrating 16 evenly spaced bins across 8kHz, aligned to lower bin
				static uint32_t f = 500 * i + 500 - 16;
				// Will land in this bin
				static uint32_t bin = 16 * i + 7;

				InputDSP::request_resetI();
				WaveGen::request_resetI();

				PT_WAIT_UNTIL(pt, InputDSP::is_resetI());
				PT_WAIT_UNTIL(pt, WaveGen::get_state() == WaveGen::ST_RESET);

				WaveGen::set_tone(0, 0, f, 0, 500, calib_tone_level);
				InputDSP::configure(256, 25, 15);

				PT_WAIT_UNTIL(pt, InputDSP::is_ready() && WaveGen::is_ready());

				timer.reset();
				WaveGen::runI();
				InputDSP::runI();
				timer.start();

				PT_WAIT_UNTIL(pt, InputDSP::get_state() == InputDSP::ST_DONE);

				coeffs[i] = (1.0 / 16.0)  / InputDSP::get_psd()[bin].asFloat();
			}
		}
	}

	PT_END(pt);
}

void APulseController::handle_eventI ( uint8_t event_type ) {
	switch(event_type){
	case USB_APP_ENUM_COMPLETE:
		err_code = 0;
		timer.reset();
		Platform::led.set();

		teststate = TEST_RESET;
		WaveGen::request_resetI();
		InputDSP::request_resetI();
		break;
	}
}

uint8_t * APulseController::get_response ( uint16_t& size ) {
	static union {
		uint8_t data[64];
		status_pkt_t status;
		uint32_t data32[16];
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
		arm_copy_q31((q31_t*)&InputDSP::get_psd()[cmd_idx++ * 16], (q31_t*)p.data, 16);
		size = 64;
		return p.data;
	case ST_GETAVG:
		arm_copy_q31((q31_t*)&InputDSP::get_average()[cmd_idx++ * 16], (q31_t*)p.data, 16);
		if(cmd_idx == 32){
			state = ST_RESET;
			cmd_idx = 0;
		}
		size = 64;
		return p.data;
	case ST_RESET:
	case ST_RESETTING:
	default:
		// Send the status packet
		p.status.version = 02;
		p.status.input_state = InputDSP::get_state();
		p.status.wavegen_state = WaveGen::get_state();
		p.status.controller_state = teststate;

// 		p.status.is_started = timer.is_running() ? 1 : 0;
// 		p.status.is_capturing = InputDSP::is_running() ? 1 : 0;
// 		p.status.is_playing = WaveGen::is_running() ? 1 : 0;
//
// 		p.status.reset_controller = (state == ST_RESET) ? 1 : 0;
// 		p.status.reset_input = InputDSP::is_resetI() ? 1 : 0;
// 		p.status.reset_wavegen = WaveGen::is_resetI() ? 1 : 0;
//
// 		p.status.test_ready = (WaveGen::is_ready() &&
// 		                       InputDSP::is_ready()) ? 1 : 0;
		p.status.err_code = err_code;

		
		size = sizeof(status_pkt_t);
		return p.data;
	}
	return nullptr;
}

void APulseController::handle_dataI ( uint8_t* data, uint8_t size ) {
	auto const a = reinterpret_cast<r_packet_t *>(data);

	// First make sure there is data to parse
	//// APPARENTLY SIZE ISN'T BEING SET RIGHT... WHATEVER...
	//if(!size)
	//	return;

	static_assert(sizeof(tone_config_t) == 11,
					"tone_config_t wrong size");
	static_assert(sizeof(tone_setup_pkt_t) == 3*sizeof(tone_config_t) + 1,
					"tone_setup_pkt_t wrong size");
	static_assert(sizeof(status_pkt_t) == 5,
					"status_pkt_t wrong size");
	static_assert(sizeof(capture_config_pkt_t) == 8,
					"capture_config_pkt wrong size");

	switch(*data){
	case CMD_RESET:
		err_code = 0;
		//timer.stop();
		//timer.reset_count();
		timer.reset();
		Platform::led.clear();

		teststate = TEST_RESET;
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
		if(teststate == TEST_RESET ||
		   teststate == TEST_CONFIGURING ||
		   teststate == TEST_READY){
			InputDSP::configure(
				a->capture_config_pkt.overlap,
				a->capture_config_pkt.start_time,
				a->capture_config_pkt.num_windows
			);
			teststate = WaveGen::is_ready() ? TEST_READY : TEST_CONFIGURING;
		}
		break;
	case CMD_SETUPTONES:
		if((teststate == TEST_RESET) ||
		   (teststate == TEST_CONFIGURING) ||
		   (teststate == TEST_READY)){
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
			// Advance to next state if got a real tone
			if(WaveGen::is_ready()){
				teststate = InputDSP::is_ready() ? TEST_READY : TEST_CONFIGURING;
			}
		}
		break;
	case CMD_STARTUP:
		break;
	case CMD_CALIBRATE_MIC:
		if(teststate == TEST_RESET)
			teststate = TEST_CALIB_MIC;
		break;
	case CMD_RESET_CALIB:
		if(teststate == TEST_RESET)
			clear_calibration();
		break;
	case CMD_START:
		err_code = 0;
		if(!WaveGen::is_ready()){
			err_code = 1;
		}
		if(!InputDSP::is_ready()){
			err_code |= 2;
		}
		if(!(teststate == TEST_READY)){
			err_code |= 4;
		}
		if(err_code)
			break;
// 		timer.stop();
// 		timer.reset_count();
		timer.reset();
		WaveGen::runI();
		InputDSP::runI();
		timer.start();
		teststate = TEST_RUNNING;
		Platform::led.set();
		break;
	default:
		return;
	}
}
