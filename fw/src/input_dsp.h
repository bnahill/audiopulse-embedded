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

#ifndef __APULSE_INPUT_DSP_H_
#define __APULSE_INPUT_DSP_H_

#include <driver/platform.h>
#include <driver/codec.h>
#include <arm_math.h>
#include <apulse_math.h>

class InputDSP {
public:
	/*!
	 @brief The protothread for performing required DSP operations
	 */
	static PT_THREAD(pt_dsp(struct pt * pt));
	
	static void configure(uint8_t overlap,
	                      uint16_t start_time_ms,
	                      uint16_t num_windows){
		InputDSP::overlap = overlap;
		InputDSP::start_time_ms = start_time_ms;
		InputDSP::num_windows = num_windows;
	}
		
	/*!
	 @brief Callback to receive new samples
	 
	 @note MUST BE CALLED FROM INTERRUPT OR LOCKED CONTEXT
	 */
	static void put_samplesI(AK4621::sample_t * samples);

	static void run(){running = true;}

	static void stop(){running = false;}

	static void request_resetI(){pending_reset = true;}

	static bool is_resetI(){return is_reset;}

	static bool is_running(){return running;}

	static bool is_ready(){
		return (num_windows > 0) &&
		       (window_count == 0) &&
		       (num_samples == 0) &&
		       (start_time_ms != -1);
	}
	
	typedef sFractional<0,31> sampleFractional;
	typedef sFractional<1,30> powerFractional;

	static constexpr powerFractional const * get_psd() {
		return mag_psd;
	}

	static constexpr sampleFractional const * get_average() {
		return average_buffer;
	}

	static constexpr uint32_t transform_len = 512;
	
protected:
	typedef AK4621::sample_t sample_t;
	
	

	static sampleFractional const * new_samples;
	// MUST HAVE PROTECTED ACCESS
	static uint32_t num_samples;

	//! @name Configuration
	//! @{
	//! Start time
	static uint32_t start_time_ms;
	static uint16_t num_windows;
	//! @}
	
	//! @name Decimation variables and configuration
	//! @{
	//! The filter order for FIR decimation
	static constexpr uint16_t decimate_fir_order = 21;
	static sample_t const decimate_coeffs[decimate_fir_order];
	// The number of samples processed in each iteration of ARM FIR decimate
	static constexpr uint16_t decimate_block_size = 48;
	// The number of output samples generated at a time
	static constexpr uint16_t decimate_output_size = 256;
	static constexpr uint32_t decimated_frame_buffer_size = 3 * transform_len / 2;
	//! 
	static sampleFractional decimated_frame_buffer[decimated_frame_buffer_size];
	//! The decimate state
	static arm_fir_decimate_instance_q31 decimate;
	static void do_decimate(sampleFractional * dst);

	static uint32_t theta;
	static uint8_t buffer_sel;

	static sampleFractional decimate_buffer[decimate_block_size +
	                                        decimate_fir_order - 1];
	//! @}
	
	//! @name Windowing and FFT variables and configuration
	//! @{
	//! The number of decimated samples available
	static uint16_t num_decimated;
	//! The current window overlap setting (M)
	static uint8_t overlap;
	//! The constant 512 sample Q31 Hamming window
	static sample_t const hamming512[512];
	static uint32_t num_before_end;
	static sampleFractional transform_buffer[transform_len];

	//! Storage of complex transform and the computed PSD
	static sampleFractional complex_transform[transform_len];

	static powerFractional mag_psd[transform_len / 2 + 1];
	
	//! The average windowed waveform
	static sampleFractional average_buffer[transform_len];

	//! The number of windows processed so far
	static uint32_t window_count;

	//! RFFT instance structure
	static arm_rfft_instance_q31 rfft;
	//! CFFT instance structure
	static arm_cfft_radix4_instance_q31 cfft;
	//! @}
	
	//! Standard reset flags
	static bool pending_reset, is_reset, running;

	struct AverageConstants {
		sampleFractional one_over;
		sampleFractional one_minus;
	};

	static AverageConstants mk_multipliers(){
		if(!window_count) return {0.0,0.0};
		sampleFractional one_over = sampleFractional::mk_frac(1, window_count);
		return {one_over, ((sampleFractional)1.0) - one_over};
	}

	static void do_reset(){
		running = 0;
		new_samples = nullptr;
		num_samples = 0;
		buffer_sel = 0;
		theta = 0;
		start_time_ms = -1;
		window_count = 0;

		// Decimated frame buffer really doesn't need to be zero'd...
		for(auto &a : decimated_frame_buffer) a.setInternal(0);

		// These ones are important though...
		for(auto &a : mag_psd) a.setInternal(0);
		for(auto &a : average_buffer) a.setInternal(0);

		arm_fir_decimate_init_q31(&decimate, 5, 3,//decimate_fir_order, 3,
	                             (q31_t*)decimate_coeffs,
	                             (q31_t*)decimate_buffer,
	                             decimate_block_size);

		is_reset = true;
		pending_reset = false;
	}
	
	static constexpr bool debug = true;
};

#endif // __APULSE_INPUT_DSP_H_
