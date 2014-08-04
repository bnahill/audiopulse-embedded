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

 @file controller.cpp
 @brief The main control logic (implementation)
 @author Ben Nahill <bnahill@gmail.com>
 */

#include <controller.h>


//////////////////////////////////////////////////////////
// Static allocations
//////////////////////////////////////////////////////////

APulseController::state_t APulseController::state;
uint32_t APulseController::cmd_idx;
uint8_t APulseController::err_code;
decltype(APulseController::teststate) APulseController::teststate = TEST_RESET;
constexpr TimerPIT APulseController::timer;
InputDSP::powerFractional APulseController::coeffs[16];

__attribute__((section(".m_data2")))
InputDSP::sampleFractional APulseController::dump_buffer[InputDSP::transform_len];

BufferDump APulseController::waveform_dump(reinterpret_cast<uint8_t const *>(dump_buffer), sizeof(dump_buffer));

uint32_t APulseController::most_recent_t_ms;


BufferDump::BufferDump(uint8_t const * _buffer, size_t _n) :
	buffer(_buffer),
	n(_n),
	state(ST_EMPTY),
	iter(_buffer)
	{}

size_t BufferDump::get_frame_copy(uint8_t * dst, size_t bytes){
	if(state == ST_EMPTY)
		return 0;
	state = ST_DUMPING;
	size_t to_copy = min(bytes, (size_t)(buffer + n) - (size_t)iter);
	memcpy((void *)dst, iter, to_copy);
	iter += to_copy;
	if(iter == (buffer + n)){
		state = ST_EMPTY;
	}
	return to_copy;
}

bool BufferDump::copy_data(uint8_t const * src){
	if(state != ST_EMPTY)
		return false;

	memcpy((void *)buffer, (void const *)src, n);
	iter = buffer;

	state = ST_FULL;
	return true;
}

bool BufferDump::set_data(uint8_t const * src){
	if(state != ST_EMPTY)
		return false;

	buffer = src;
	iter = buffer;
	state = ST_FULL;
	return true;
}




PT_THREAD(APulseController::pt_controller)(struct pt * pt){
	static uint32_t i;

	//waveform_dump = BufferDump(reinterpret_cast<uint8_t const *>(dump_buffer),sizeof(dump_buffer));

	PT_BEGIN(pt);

	clear_calibration();

	timer.reset();

	// Run from 32kHz/32 clock
	//timer.configure(Timer::CLKS_FIXED, Timer::PS_32, 0);
	//timer.reset_count(0);

	while(true){
		static constexpr float bins_f = 16.0;
		static constexpr uint32_t bins = 16;

		static_assert(sizeof(coeffs) / sizeof(*coeffs) >= bins,
					  "Calibration coefficient storage insufficient");

		PT_YIELD(pt);

		// Is a test completed?
		if(InputDSP::get_state() == InputDSP::ST_DONE &&
		   WaveGen::get_state() == WaveGen::ST_DONE){
			Platform::leds[1].clear();
			// Clean up after test
			teststate = TEST_DONE;
			// Disable analog supply
			//Platform::power_en.clear();
			if(timer.is_running())
				timer.reset();
		}

		// Is it time to calibrate the microphone?
		if(teststate == TEST_CALIB_MIC){
			// Analog supply was already enabled in USB driver

			// Delay for startup
			timer.reset();
			timer.start();
			PT_WAIT_UNTIL(pt, timer.get_ms() > 50);
			timer.reset();

			TPA6130A2::enable();
			AK4621::init();

			// Delay for init
			timer.reset();
			timer.start();
			PT_WAIT_UNTIL(pt, timer.get_ms() > 50);
			timer.reset();

			for(i = 0; i < bins; i++){
				// Calibrating {bins} evenly spaced bins across 8kHz,
				// aligned to lower bin
				static uint32_t f = (8000 / bins) * i + (8000 / bins) - bins;
				// Will land in this bin
				static uint32_t bin = bins * i + (bins / 2 - 1);

				InputDSP::request_resetI();
				WaveGen::request_resetI();

				PT_WAIT_UNTIL(pt, InputDSP::is_resetI());
				PT_WAIT_UNTIL(pt, WaveGen::get_state() == WaveGen::ST_RESET);

				WaveGen::set_tone(0, 0, f, 0, 500, calib_tone_level);
				InputDSP::configure(256, 25, 15, AK4621::Src::MIC, 0.0, 0.0);

				PT_WAIT_UNTIL(pt, InputDSP::is_ready() && WaveGen::is_ready());

				timer.reset();
				WaveGen::runI();
				InputDSP::runI();
				timer.start();

				PT_WAIT_UNTIL(pt, InputDSP::get_state() == InputDSP::ST_DONE);

				// And this should be the only actualy floating-point math
				coeffs[i] = (1.0 / bins_f) / InputDSP::get_psd()[bin].asFloat();
			}

			Platform::power_en.clear();
			// TODO: Is this the correct transition?
			teststate = TEST_DONE;
		}

		// Should we start a test?
		if(teststate == TEST_STARTING){
			// Supply was already enabled
			// Delay for startup
			Platform::leds[1].set();
			//Platform::power_en.set();
			timer.reset();
			timer.start();
			PT_WAIT_UNTIL(pt, timer.get_ms() > 50);

			TPA6130A2::enable();
			AK4621::init_hw();
			AK4621::init();
			AK4621::start();

			// Delay for init
			timer.reset();
			timer.start();
			PT_WAIT_UNTIL(pt, timer.get_ms() > 50);
			timer.reset();

			waveform_dump.reset();
			WaveGen::runI();
			InputDSP::runI();
			timer.start();
			teststate = TEST_RUNNING;
		}
	}

	PT_END(pt);
}

void APulseController::handle_eventI ( uint8_t event_type ) {
	switch(event_type){
	case USB_APP_ENUM_COMPLETE:
		err_code = 0;
		timer.reset();

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
	case ST_DUMPWAVE:
		if(do_buffer_dumps and waveform_dump.has_data()){
			waveform_dump.get_frame_copy(p.data, 64);
		} else {
			size = 0;
			state = ST_RESET;
			return p.data;
		}
		if(!waveform_dump.has_data()){
			state = ST_RESET;
		}
		size = 64;
		return p.data;
	case ST_RESET:
	case ST_RESETTING:
	default:
		// Send the status packet
		p.status.version = protocol_version;
		p.status.input_state = InputDSP::get_state();
		p.status.wavegen_state = WaveGen::get_state();
		p.status.controller_state = teststate;

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
	static_assert(sizeof(capture_config_pkt_t) == 16,
					"capture_config_pkt wrong size");

	switch(*data){
	case CMD_RESET:
		err_code = 0;
		//timer.stop();
		//timer.reset_count();
		timer.reset();
		waveform_dump.reset();

		teststate = TEST_RESET;
		Platform::leds[1].clear();
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

			if(!AK4621::is_Src(a->capture_config_pkt.source))
				break;

			InputDSP::configure(
				a->capture_config_pkt.overlap,
				a->capture_config_pkt.start_time,
				a->capture_config_pkt.num_windows,
				a->capture_config_pkt.source,
				a->capture_config_pkt.scale_mic,
				a->capture_config_pkt.scale_ext
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
			Platform::power_en.set();
			teststate = TEST_CALIB_MIC;
		break;
	case CMD_RESET_CALIB:
		if(teststate == TEST_RESET)
			clear_calibration();
		break;
	case CMD_START:
		err_code = 0;
		if(!WaveGen::is_ready()){
			err_code = ERR_WAVEGEN;
		}
		if(!InputDSP::is_ready()){
			err_code |= ERR_INPUT;
		}
		if(!(teststate == TEST_READY)){
			err_code |= ERR_TEST;
		}
		if(err_code)
			break;
		Platform::power_en.set();
		teststate = TEST_STARTING;
		break;
	case CMD_PULLWAVEFORM:
		if(state == ST_RESET){
			state = ST_DUMPWAVE;
		}
		break;
	default:
		return;
	}
}
