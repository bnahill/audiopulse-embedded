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

class InputDSP {
public:
	/*!
	 @brief The protothread for performing required DSP operations
	 */
	static PT_THREAD(pt_dsp(struct pt * pt));
	
	static void set_overlap(uint8_t overlap){InputDSP::overlap = overlap;}
		
	/*!
	 @brief Callback to receive new samples
	 
	 @note MUST BE CALLED FROM INTERRUPT OR LOCKED CONTEXT
	 */
	static void put_samplesI(AK4621::sample_t * samples);
protected:
	typedef AK4621::sample_t sample_t;
	
	static AK4621::sample_t const * new_samples;
	// MUST HAVE PROTECTED ACCESS
	static uint32_t num_samples;
	
	//! @name Decimation variables and configuration
	//! @{
	static constexpr uint16_t decimate_fir_order = 21;
	static sample_t const decimate_coeffs[decimate_fir_order];
	static constexpr uint16_t decimate_block_size = 48;
	//! 
	static sample_t decimated_frame_buffer[512];
	//! The decimate state
	static arm_fir_decimate_instance_q31 decimate;
	static void do_decimate(sample_t * dst);
	//! @}
	
	//! @name Windowing and FFT variables and configuration
	//! @{
	//! The number of decimated samples available
	static uint16_t num_decimated;
	//! The current window overlap setting (M)
	static uint8_t overlap;
	//! The constant 256 sample Q31 Hamming window
	static sample_t const hamming256[256];
	static void do_transform();
	static void do_average();
	//! @}
	
	
	static constexpr bool debug = true;
};

#endif // __APULSE_INPUT_DSP_H_