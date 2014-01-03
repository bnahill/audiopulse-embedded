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
	
	static void configute(uint8_t overlap, uint16_t start_time_ms){
		InputDSP::overlap = overlap;
		InputDSP::start_time_ms = start_time_ms;
	}
		
	/*!
	 @brief Callback to receive new samples
	 
	 @note MUST BE CALLED FROM INTERRUPT OR LOCKED CONTEXT
	 */
	static void put_samplesI(AK4621::sample_t * samples);

	static void request_resetI(){
		pending_reset = true;
	}

	static bool is_resetI(){
		return is_reset;
	}
	
	typedef sFractional<0,31> sampleFractional;
	typedef sFractional<1,30> powerFractional;

	static constexpr sampleFractional const * get_transform() {
		return transform.complex;
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

	//! Start time
	static uint16_t start_time_ms;
	
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

	static union TransformOut {
		sampleFractional complex[transform_len];
		powerFractional power[transform_len / 2 - 1];
		TransformOut(){}
	} transform;
	
	static sampleFractional average_buffer[transform_len];
	static sampleFractional const one_over_len;
	static sampleFractional const len_minus_one_over_len;

	static arm_rfft_instance_q31 rfft;
	static arm_cfft_radix4_instance_q31 cfft;
	//! @}
	
	static bool pending_reset, is_reset;

	static void do_reset(){
		num_samples = 0;
		buffer_sel = 0;
		theta = 0;
		start_time_ms = -1;

		// Decimated frame buffer really doesn't need to be zero'd...
		for(auto &a : decimated_frame_buffer) a.setInternal(0);

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
