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

#include <input_dsp.h>
#include <arm_math.h>
#include <apulse_math.h>
#include <controller.h>
#include <pt-sem.h>
#include <driver/usb.h>

//InputDSP::sampleFractional const * volatile InputDSP::new_samples  = nullptr;
uint32_t volatile InputDSP::num_samples = 0;
uint32_t volatile InputDSP::num_received = 0;

uint16_t InputDSP::num_decimated;

decltype(InputDSP::start_time_ms) InputDSP::start_time_ms = -1;
decltype(InputDSP::end_time_ms) InputDSP::end_time_ms = -1;

bool InputDSP::is_reset, InputDSP::pending_reset_capture, InputDSP::pending_reset_dsp;

uint32_t InputDSP::num_before_end = 0;

arm_rfft_fast_instance_f32 InputDSP::rfft;

__attribute__((section(".m_data2")))
InputDSP::sampleFractional InputDSP::transform_buffer[transform_len];

__attribute__((section(".m_data2")))
decltype(InputDSP::complex_transform) InputDSP::complex_transform;

__attribute__((section(".m_data2")))
decltype(InputDSP::mag_psd) InputDSP::mag_psd;

__attribute__((section(".m_data2")))
InputDSP::sampleFractional InputDSP::average_buffer[transform_len];

uint32_t InputDSP::window_count;

decltype(InputDSP::overlap) InputDSP::overlap;

decltype(InputDSP::state) InputDSP::state = ST_UNKNOWN;

decltype(InputDSP::max) InputDSP::max, InputDSP::min, InputDSP::in_min, InputDSP::in_max;

decltype(InputDSP::range_psd) InputDSP::range_psd;
decltype(InputDSP::range_in) InputDSP::range_in;
decltype(InputDSP::range_decimated) InputDSP::range_decimated;

AK4621::Src InputDSP::src;
InputDSP::sampleFractional InputDSP::scale_mic;
InputDSP::sampleFractional InputDSP::scale_ext;

InputDSP::flags_t volatile InputDSP::flags;
InputDSP::input_t volatile InputDSP::input;


void InputDSP::configure(uint16_t overlap,
						 uint16_t start_time_ms,
						 uint16_t end_time_ms,
						 AK4621::Src src,
						 sampleFractional scale_mic,
						 sampleFractional scale_ext){
	if(state == ST_RESET || state == ST_READY){
		is_reset = false;
		InputDSP::overlap = overlap;
		InputDSP::start_time_ms = start_time_ms;
		InputDSP::end_time_ms = end_time_ms;
		InputDSP::src = src;
		InputDSP::scale_mic = scale_mic;
		InputDSP::scale_ext = scale_ext;
		state = ST_READY;
	}
}

InputDSP::Decimator InputDSP::decimator;


PT_THREAD(InputDSP::pt_capture_decimate(struct pt * pt)){
	static decltype(input.samples) old_new_samples;

	PT_BEGIN(pt);

	// Always
	while(true){
		flags.pending_reset_capture = 0;
		// Per capture stream
		PT_WAIT_UNTIL(pt, flags.go_capture or flags.pending_reset_capture);
		
		
		if(flags.pending_reset_capture)
			continue;
		
		input.count = 0;
		
		////////////////////////////
		// Configure CODEC
		////////////////////////////

		Platform::codec.set_in_cb(put_samplesI);

		////////////////////////////
		// Prepare FIR decimation
		////////////////////////////

		static_assert(AK4621::in_buffer_size == 512 * 3, "Wrong buffer size for DSP");
		
		while(true){
			PT_WAIT_UNTIL(pt, flags.pending_reset_capture or input.count);
				
			if(flags.pending_reset_capture)
				break;

            auto n_going_to_decimate = input.count;
            auto samples_going_to_decimated = input.samples;

            input.count = 0;

            num_samples += n_going_to_decimate;
			old_new_samples = input.samples;

            decimator.decimate(samples_going_to_decimated, n_going_to_decimate);

            if(n_going_to_decimate / decimator.decimate_factor != 512)
                while(true);

            while(USB::audioReadyForData() && decimator.numConsecutiveAvailableUSB()){
				// Take however many samples there are and send them over USB
                auto nsamples = decimator.numConsecutiveAvailableUSB();
                //auto nsamples = n_going_to_decimate / decimator.decimate_factor;
				nsamples = ::min(nsamples, USB::audioMaxSamples);

                if(USB::audioSend(decimator.getDecimatedPtrUSB(), nsamples)){
                    decimator.advanceUSB(nsamples);
                } else {
                    break;
                }

            }
			
			// This is all if we don't need to process the samples
			if(state != ST_CAPTURING)
				continue;		}
	}
	PT_END(pt);
}

PT_THREAD(InputDSP::pt_dsp(struct pt * pt)){
	
	static AverageConstants constants;

	PT_BEGIN(pt);

	////////////////////////////
	// Reset buffers
	////////////////////////////

	do_reset();

	////////////////////////////
	// Prepare FFT
	////////////////////////////
	{
		// Contain the scope of status
		auto status = arm_rfft_fast_init_f32(&rfft, transform_len);
		while(status != ARM_MATH_SUCCESS);
	}

	while(true){
		// Just wait until we are supposed ot be waiting for the right time
		PT_WAIT_UNTIL(pt, pending_reset_dsp || (state == ST_RUNWAIT));
		if(pending_reset_dsp){ do_reset(); continue; }

		// Now wait for the right time
		PT_WAIT_UNTIL(pt, pending_reset_dsp ||
						  APulseController::get_time_ms() > start_time_ms);
		if(pending_reset_dsp){ do_reset(); continue; }

		// Switch to the correct source and reset buffers
        Platform::codec.set_source(src, scale_mic, scale_ext);

		state = ST_CAPTURING;
		Platform::leds[0].set();

		range_psd.reset();
		range_decimated.reset();
		range_in.reset();

		// Wait for new samples and then discard them. This is easier than
		// resetting DMA but...
		// TODO: Make setting source reset DMA (but this is still safe
        PT_WAIT_UNTIL(pt, decimator.numAvailable() >= transform_len);

		static sampleFractional window_scale = 1.0;// / (float)num_windows;

		while(state == ST_CAPTURING){

			num_decimated += num_received / 3;

			PT_YIELD(pt);

			// While there is a full transform available...
			while(decimator.numAvailable() >= transform_len){
				// THIS IS RUN ONCE PER EPOCH
				constants = mk_multipliers();

				// The number of samples remaining before the end of the decimated_frame_buffer
				//num_before_end = decimated_frame_buffer_size - decimation_read_head;
				num_before_end = decimator.numConsecutiveAvailable();
				if(num_before_end < transform_len){
					// Split in two

					if(use_rectangular){
						arm_copy_f32(decimator.getDecimatedPtr(),
						             (float32_t*)transform_buffer,
									 num_before_end);
					} else {
						arm_mult_f32(decimator.getDecimatedPtr(),
									(float32_t*)hamming_window, (float32_t*)transform_buffer,
									 num_before_end);
					}
					weighted_vector_sum(
						constants.one_over,
						decimator.getDecimatedPtr(),
						constants.one_minus,
						average_buffer,
						average_buffer,
						num_before_end
					);
					// Do second half
					if(use_rectangular){
						arm_copy_f32(decimator.getDecimatedPtr(num_before_end),
									 (float32_t*)&transform_buffer[num_before_end],
									 transform_len - num_before_end);
					} else {
						arm_mult_f32(decimator.getDecimatedPtr(num_before_end),
									 (float32_t*)&hamming_window[num_before_end],
									 (float32_t*)&transform_buffer[num_before_end],
									 transform_len - num_before_end);
					}
					weighted_vector_sum(
						constants.one_over,
						decimator.getDecimatedPtr(num_before_end),
						constants.one_minus,
						&average_buffer[num_before_end],
						&average_buffer[num_before_end],
						transform_len - num_before_end
					);
				} else {
					// All in one shot
					if(use_rectangular){
						arm_copy_f32(
							decimator.getDecimatedPtr(),
							(float32_t*)transform_buffer,
							transform_len
						);
					} else {
						arm_mult_f32(
							decimator.getDecimatedPtr(),
							(float32_t*)hamming_window,
							(float32_t*)transform_buffer,
							transform_len
						);
					}

					weighted_vector_sum(
						constants.one_over,
						decimator.getDecimatedPtr(),
						constants.one_minus,
						average_buffer,
						average_buffer,
						transform_len
					);
				}

				// Advance the processing read head
				decimator.advance(transform_len - overlap);
				
				for(uint32_t i = 0; i < transform_len; i++){
					if(transform_buffer[i] > in_max)
						in_max = transform_buffer[i];
					if(transform_buffer[i] < in_min)
						in_min = transform_buffer[i];
				}

				if(APulseController::do_buffer_dumps){
					APulseController::waveform_dump.copy_data((uint8_t const *)transform_buffer);
				}


				for(uint32_t i = 0; i < transform_len; i++){
					range_decimated.check(transform_buffer[i]);
				}

				PT_YIELD(pt);

				arm_rfft_fast_f32(&rfft, (float32_t*)transform_buffer,
							      (float32_t*)complex_transform, 0);

				PT_YIELD(pt);

				for(uint32_t i = 0; i < transform_len; i++){
					range_psd.check(complex_transform[i]);
				}

//				complex_psd_acc(
//					complex_transform,
//					mag_psd,
//					mag_psd,
//					transform_len + 2
//				);

				complex_psd_mac(
					window_scale,
					complex_transform,
					mag_psd,
					mag_psd,
					transform_len + 2
				);
// 				complex_power_avg_update(
// 					(powerFractional)constants.one_over,
// 					complex_transform,
// 					(powerFractional)constants.one_minus,
// 					mag_psd,
// 					mag_psd,
// 					transform_len + 2
// 				);

				// Check if we've processed enough windows to shut down
				window_count += 1;
				if(APulseController::get_time_ms() > end_time_ms){
				//if(++window_count >= num_windows){
					if(calibrate_mic){
						for(uint32_t i = 0; i < transform_len / 2; i++){
							mag_psd[i] = mag_psd[i] * APulseController::coeffs[i / 16];
						}
						mag_psd[transform_len / 2] = mag_psd[transform_len / 2] * APulseController::coeffs[15];
					}
					vector_mult_scalar<powerFractional>(1.0 / window_count, mag_psd, mag_psd,
					                                    transform_len / 2 + 1);
//					vector_mult_scalar(window_scale, mag_psd, mag_psd, transform_len/2 + 1);
					state = ST_DONE;
					Platform::leds[0].clear();
					break;
				}

				PT_YIELD(pt);

			}
		}
	}

	PT_END(pt);
}


void InputDSP::do_reset(){
	num_samples = 0;
	num_received = 0;
	num_decimated = 0;
	start_time_ms = -1;
	window_count = 0;

	max = 0;
	min = 0;
	Platform::leds[0].clear();

	// These ones are important though...
	for(auto &a : mag_psd) a = 0;
	for(auto &a : average_buffer) a = 0;


	is_reset = true;
    pending_reset_dsp = false;

	state = ST_RESET;
}

void InputDSP::put_samplesI(sample_t * samples, size_t n){
	arm_q31_to_float(reinterpret_cast<q31_t*>(samples), reinterpret_cast<float*>(samples), n);
	
	input.samples = reinterpret_cast<sampleFractional *>(samples);
    //num_received = n;
	//for(uint32_t i = 0; i < n; i++){
	//	range_in.check(new_samples[i]);
	//}
	
	input.count = n;
}

void InputDSP::Decimator::reset_priv(){
	decimated_proc_read_head = 0;
	decimated_write_head = 0;
	decimated_count_proc = 0;
	decimated_count_usb = 0;
	decimated_usb_read_head = 0;
	do_reset = false;
	if(do_filter){
		if(use_iir) {
			arm_biquad_cascade_df1_init_f32(&biquad_cascade, biquad_stages,
											(float32_t *)biquad_coeffs, biquad_state);
		} else {
			auto result =
			arm_fir_decimate_init_f32(&decimate_inst, decimate_fir_order, 3,
			                          (float32_t*)decimate_coeffs,
			                          (float32_t*)decimate_buffer,
			                          decimate_block_size);

			while(result != ARM_MATH_SUCCESS);
		}
	}	
};

InputDSP::sampleFractional *
InputDSP::Decimator::decimate(sampleFractional const * src,
	                          size_t n_in){

    static float tmp_val = 0.0;

	sampleFractional const * iter_in;
	sampleFractional * dst = &decimate_buffer[decimated_write_head];
	auto retval = dst;
	iter_in = src;
    uint32_t n_out = n_in / decimate_factor;

	if(do_filter){
		for(auto i = 0; i < (n_in / decimate_block_size); i++){
			//arm_shift_q31((q31_t*)iter_in, -6, (q31_t*)iter_in, decimate_block_size);
			if(use_iir) {
				biquad_cascade_f32_decimate<decimate_block_size / decimate_factor, decimate_factor>(
					biquad_cascade, (float32_t *)iter_in, (float32_t *)dst);
			} else {
				arm_fir_decimate_f32(&decimate_inst, (float32_t*)iter_in,
				                     (float32_t*)dst, decimate_block_size);
			}
			iter_in += decimate_block_size;
			dst += decimate_block_size / decimate_factor;
		}
    } else if (true) {
        for(uint32_t i = 0; i < n_out; i++){
            dst[i] = tmp_val;
            tmp_val += (1.0 / 16000.0);
            if(tmp_val >= 0.5){
                tmp_val = -0.5;
            }
        }
    } else {
        for(auto i = 0; i < n_out; i++){
			*dst = *iter_in;

			iter_in += decimate_factor;
			dst += 1;
		}
	}

	uint32_t space_left = decimated_frame_buffer_size - decimated_count_proc;
	if(space_left < n_out){
		// Push the read head forward if we overwrite it.
		decimated_proc_read_head += n_out - space_left;
		if(decimated_proc_read_head >= decimated_frame_buffer_size)
			decimated_proc_read_head -= decimated_frame_buffer_size;
		// Limit the count at the buffer size;
		decimated_count_proc = decimated_frame_buffer_size;
	} else {
		decimated_count_proc += n_out;
	}

	uint32_t space_left_usb = decimated_frame_buffer_size - decimated_count_usb;
	if(space_left_usb < n_out){
        // Push the read head forward if we overwrite it.
		decimated_usb_read_head += n_out - space_left_usb;
		if(decimated_usb_read_head >= decimated_frame_buffer_size)
			decimated_usb_read_head -= decimated_frame_buffer_size;
		// Limit the count at the buffer size;
        decimated_count_usb = decimated_frame_buffer_size;
	} else {
        decimated_count_usb += n_out;
	}

	// Always shift the write pointer by the same amount
    decimated_write_head += n_out;
	static_assert(decimated_frame_buffer_size == 1536, "Decimation size has changed");
	if(decimated_write_head >= decimated_frame_buffer_size)
		decimated_write_head -= decimated_frame_buffer_size;
	
	return retval;
}

InputDSP::sampleFractional * InputDSP::Decimator::getDecimatedPtr(size_t offset){
	// Get the return values
	if(numConsecutiveAvailable() >= offset){
		return &decimated_frame_buffer[decimated_proc_read_head + offset];
	} else {
		return &decimated_frame_buffer[decimated_proc_read_head + offset - decimated_frame_buffer_size];
	}
}

InputDSP::sampleFractional * InputDSP::Decimator::getDecimatedPtrUSB(size_t offset){
	// Get the return values
	if(numConsecutiveAvailableUSB() >= offset){
		return &decimated_frame_buffer[decimated_usb_read_head + offset];
	} else {
		return &decimated_frame_buffer[decimated_usb_read_head + offset - decimated_frame_buffer_size];
	}
}

void InputDSP::Decimator::advance(size_t n){
	decimated_proc_read_head += n;
    if(decimated_proc_read_head >= decimated_frame_buffer_size)
        decimated_proc_read_head -= decimated_frame_buffer_size;
	decimated_count_proc -=  n;
}

void InputDSP::Decimator::advanceUSB(size_t n){
	decimated_usb_read_head += n;
    if(decimated_usb_read_head >= decimated_frame_buffer_size)
        decimated_usb_read_head -= decimated_frame_buffer_size;
	decimated_count_usb -=  n;
}
