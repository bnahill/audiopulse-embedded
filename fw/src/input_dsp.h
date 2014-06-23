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
	typedef AK4621::sample_t sample_t;
	
	/*!
	 @brief The protothread for performing required DSP operations
	 */
	static PT_THREAD(pt_dsp(struct pt * pt));
	
	static void configure(uint16_t overlap,
	                      uint16_t start_time_ms,
	                      uint16_t num_windows,
	                      AK4621::Src src,
	                      sFractional<0,31> scale_mic,
	                      sFractional<0,31> scale_ext);
		
	/*!
	 @brief Callback to receive new samples
	 
	 @note MUST BE CALLED FROM INTERRUPT OR LOCKED CONTEXT
	 */
	static void put_samplesI(sample_t * samples, size_t n);

	/*!
	 @brief Transition from READY to RUN state

	 @note MUST BE CALLED FROM INTERRUPT OR LOCKED CONTEXT
	 */
	static inline void runI(){
		if(state == ST_READY){
			state = ST_RUNWAIT;
		}
	}

	static inline void request_resetI(){pending_reset = true;}

	static inline bool is_resetI(){return is_reset;}

	static inline bool is_ready(){
		return state == ST_READY;
// 		return (num_windows > 0) &&
// 		       (window_count == 0) &&
// 		       (num_samples == 0) &&
// 		       (start_time_ms != -1);
	}
	
	typedef sFractional<0,31> sampleFractional;
	typedef sFractional<6,25> powerFractional;
	typedef sFractional<8,23> transformFractional;

	static constexpr powerFractional const * get_psd() {
		return mag_psd;
	}

	static constexpr sampleFractional const * get_average() {
		return average_buffer;
	}

	static constexpr uint32_t transform_len = 512;
	
	typedef enum {
		ST_RESET     = 0,
		ST_READY     = 1,
		ST_RUNWAIT   = 2,
		ST_CAPTURING = 3,
		ST_DONE      = 4,
		ST_UNKNOWN   = 5
	} state_t;

	static inline const state_t& get_state(){return state;}

protected:
	//! The current state in the test state machine
	static state_t state;

	//! A pointer to new samples
	static sampleFractional const * new_samples;
	//! Number of raw samples provided
	static uint32_t num_received;
	// MUST HAVE PROTECTED ACCESS
	static uint32_t num_samples;

	//! @name Test Configuration
	//! @{
	//! Start time
	static uint32_t start_time_ms;
	//! The number of windows to use
	static uint16_t num_windows;
	static AK4621::Src src;
	static sampleFractional scale_mic;
	static sampleFractional scale_ext;
	//! @}
	
	//! @name Decimation variables and configuration
	//! @{
	//! The filter order for FIR decimation
	static constexpr uint16_t decimate_fir_order = 41;
	static sample_t const decimate_coeffs[decimate_fir_order];
	//! The number of samples processed in each iteration of ARM FIR decimate
	static constexpr uint16_t decimate_block_size = 48;
	//! The number of output samples generated at a time
	static constexpr uint16_t decimate_output_size = 256;
	//! The size of the buffer containing decimated samples
	static constexpr uint32_t decimated_frame_buffer_size = 3 * transform_len / 2;
	//! The buffer containing decimated samples
	static sampleFractional decimated_frame_buffer[decimated_frame_buffer_size];
	//! The decimate state
	static arm_fir_decimate_instance_q31 decimate;
	//! Perform the decimation
	static void do_decimate(sampleFractional const * src,
	                        sampleFractional * dst,
							size_t n_in);
	//! An iterator for writing to the decimation buffer
	static uint32_t decimation_write_head;

	//! An iterator through the decimated buffer
	//! Incremented every transform/averaging frame
	static uint32_t decimation_read_head;
	//! Since inputs are of fixed sizes, they are placed sequentially in three
	//! buffers. This selects between them.
	static uint8_t buffer_sel;

	//! A buffer used internally by the decimation process
	static sampleFractional decimate_buffer[decimate_block_size +
	                                        decimate_fir_order - 1];
	//! @}
	
	//! @name Windowing and FFT variables and configuration
	//! @{
	//! The number of decimated samples available
	static uint16_t num_decimated;
	//! The current window overlap setting (M)
	static uint16_t overlap;
	//! The constant 512 sample Q31 Hamming window
	static sample_t const hamming512[512];
	static uint32_t num_before_end;
	static sampleFractional transform_buffer[transform_len];

	//! Storage of complex transform and the computed PSD
	static transformFractional complex_transform[transform_len + 2];

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
	static bool pending_reset, is_reset;

	struct AverageConstants {
		sampleFractional one_over;
		sampleFractional one_minus;
	};

	/*!
	 @brief Generate 1/N and 1-(1/N) multipliers to be used for this epoch
	 */
	static AverageConstants mk_multipliers(){
		//if(!window_count) return {1.0,0.0};
		sampleFractional one_over = sampleFractional::mk_frac(1, window_count + 1);
		return {one_over, ((sampleFractional)1.0) - one_over};
	}

	/*!
	 @brief Reset the whole state of the input DSP block
	 */
	static void do_reset();
	
	static constexpr bool debug = true;
};

#endif // __APULSE_INPUT_DSP_H_
