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

InputDSP::sampleFractional const * volatile InputDSP::new_samples  = nullptr;
uint32_t volatile InputDSP::num_samples = 0;
uint32_t volatile InputDSP::num_received = 0;

__attribute__((aligned(512)))
__attribute__((section(".m_data2")))
InputDSP::sampleFractional InputDSP::decimated_frame_buffer[decimated_frame_buffer_size];

uint16_t InputDSP::num_decimated;

arm_fir_decimate_instance_q31 InputDSP::decimate;

decltype(InputDSP::start_time_ms) InputDSP::start_time_ms = -1;
decltype(InputDSP::num_windows) InputDSP::num_windows;

bool InputDSP::is_reset, InputDSP::pending_reset;

uint32_t InputDSP::decimation_read_head = 0;
uint32_t InputDSP::decimation_write_head = 0;
uint32_t InputDSP::num_before_end = 0;


arm_rfft_instance_q31 InputDSP::rfft;
arm_cfft_radix4_instance_q31 InputDSP::cfft;

__attribute__((section(".m_data2")))
InputDSP::sampleFractional InputDSP::transform_buffer[transform_len];

__attribute__((section(".m_data2")))
decltype(InputDSP::complex_transform) InputDSP::complex_transform;

__attribute__((section(".m_data2")))
decltype(InputDSP::mag_psd) InputDSP::mag_psd;

__attribute__((section(".m_data2")))
InputDSP::sampleFractional InputDSP::average_buffer[transform_len];

uint32_t InputDSP::window_count;

__attribute__((section(".m_data2")))
InputDSP::sampleFractional InputDSP::decimate_buffer[decimate_block_size +
													 decimate_fir_order - 1];

decltype(InputDSP::overlap) InputDSP::overlap;

decltype(InputDSP::state) InputDSP::state = ST_UNKNOWN;

decltype(InputDSP::max) InputDSP::max, InputDSP::min, InputDSP::in_min, InputDSP::in_max;

decltype(InputDSP::range_psd) InputDSP::range_psd;
decltype(InputDSP::range_in) InputDSP::range_in;
decltype(InputDSP::range_decimated) InputDSP::range_decimated;

AK4621::Src InputDSP::src;
InputDSP::sampleFractional InputDSP::scale_mic;
InputDSP::sampleFractional InputDSP::scale_ext;

InputDSP::coeffFractional InputDSP::biquad_coeffs[] = {
    0.367872027680277824, -0.713896634057164192, 0.353304294869303703,
    -1.898041920736432076, 0.936312768608331680,

    0.367872027680277824, -0.359298091381788254, 0.353304294869303703,
    -1.327612036839127541, 0.876229001209139824,

    0.367872027680277824, -0.719211803749203682, 0.353304294869303703,
    -1.759688029065728188, 0.831335147842764854,

    0.367872027680277824, 0.099177891388535500, 0.353304294869303703,
    -1.438843607902526855, 0.718513110652565956
};

decltype(InputDSP::biquad_cascade) InputDSP::biquad_cascade;
decltype(InputDSP::biquad_state) InputDSP::biquad_state;

/**
 * Generated coefficients from http://t-filter.appspot.com/fir/index.html
 * Passband 0-6kHz -0.5dB with 1dB ripple (max 0dB)
 * Stopband 8kHz-24kHz 15dB attenuation
 */
// InputDSP::sample_t const InputDSP::decimate_coeffs[decimate_fir_order] = {
// 	-206371169, 232918033, 166081815, 48163641, -121174027, -240290533,
// 	-172723375, 139749183, 612254038, 1040027243, 1212012007, 1040027243,
// 	612254038, 139749183, -172723375, -240290533, -121174027, 48163641,
// 	166081815, 232918033, -206371169
// };

//InputDSP::sample_t const InputDSP::decimate_coeffs[decimate_fir_order] = {1.0};


InputDSP::sample_t const InputDSP::decimate_coeffs[decimate_fir_order] = {
	0.04300687444857404,
	-0.018316535246320275,
	-0.04279097712851117,
	-0.05419721797427074,
	-0.026381046923385217,
	0.048819476840206086,
	0.15138707419084213,
	0.24052665493662193,
	0.2758268944131498,
	0.24052665493662193,
	0.15138707419084213,
	0.048819476840206086,
	-0.026381046923385217,
	-0.05419721797427074,
	-0.04279097712851117,
	-0.018316535246320275,
	0.04300687444857404
};

// Bandpass ws0 = [0,200Hz]*-20dB, wp0 = [
// InputDSP::sample_t const InputDSP::decimate_coeffs[decimate_fir_order] = {
// 	-0.02435313838961002, -0.0220241968035626, -0.028160178296642625,
// 	-0.033992432830340244, -0.03934061821690012, -0.04166000024215076,
// 	-0.03654262539112184, -0.02191921866926793, -0.0025725540727438248,
// 	0.009584734164944177, 0.002000557692349713, -0.027870634434669937,
// 	-0.06561824539362501, -0.08373659608090302, -0.05655917114251735,
// 	0.02181442068737953, 0.12876507320258662, 0.2212737435328349,
// 	0.2578064564607543, 0.2212737435328349, 0.12876507320258662,
// 	0.02181442068737953, -0.05655917114251735, -0.08373659608090302,
// 	-0.06561824539362501, -0.027870634434669937, 0.002000557692349713,
// 	0.009584734164944177, -0.0025725540727438248, -0.02191921866926793,
// 	-0.03654262539112184, -0.04166000024215076, -0.03934061821690012,
// 	-0.033992432830340244, -0.028160178296642625, -0.0220241968035626,
// 	-0.02435313838961002
// };

// 41
//InputDSP::sample_t const InputDSP::decimate_coeffs[decimate_fir_order] = {
//	-29719891, -22214914, -4305276, 28510937, 55098416, 51210097, 10347956,
//	-44183282, -69791987, -35719183, 46385596, 119099633, 113845418, 5597554,
//	-150985522, -237046029, -136950297, 180834943, 623954165, 1010548673,
//	1163721112, 1010548673, 623954165, 180834943, -136950297, -237046029,
//	-150985522, 5597554, 113845418, 119099633, 46385596, -35719183, -69791987,
//	-44183282, 10347956, 51210097, 55098416, 28510937, -4305276, -22214914,
//	-29719891
//};



void InputDSP::configure(uint16_t overlap,
						 uint16_t start_time_ms,
						 uint16_t num_windows,
						 AK4621::Src src,
						 sFractional<0,31> scale_mic,
						 sFractional<0,31> scale_ext){
	if(state == ST_RESET || state == ST_READY){
		is_reset = false;
		InputDSP::overlap = overlap;
		InputDSP::start_time_ms = start_time_ms;
		InputDSP::num_windows = num_windows;
		InputDSP::src = src;
		InputDSP::scale_mic = scale_mic;
		InputDSP::scale_ext = scale_ext;
		state = ST_READY;
	}
}

PT_THREAD(InputDSP::pt_dsp(struct pt * pt)){
	static decltype(new_samples) old_new_samples;
	static AverageConstants constants;

	PT_BEGIN(pt);

	////////////////////////////
	// Reset buffers
	////////////////////////////

	do_reset();

	////////////////////////////
	// Configure CODEC
	////////////////////////////

	//AK4621::set_source(AK4621::Src::MIC);
	AK4621::set_in_cb(put_samplesI);

	////////////////////////////
	// Prepare FIR decimation
	////////////////////////////

	static_assert(AK4621::in_buffer_size == 768, "Wrong buffer size for DSP");

	////////////////////////////
	// Prepare FFT
	////////////////////////////

	arm_rfft_init_q31(&rfft, &cfft, transform_len, 0, 1);

	while(true){
		// Just wait until we are supposed ot be waiting for the right time
		PT_WAIT_UNTIL(pt, pending_reset || (state == ST_RUNWAIT));
		if(pending_reset){ do_reset(); continue; }

		// Now wait for the right time
		PT_WAIT_UNTIL(pt, pending_reset ||
						  APulseController::get_time_ms() > start_time_ms);
		if(pending_reset){ do_reset(); continue; }

		// Switch to the correct source and reset buffers
		AK4621::set_source(src, scale_mic, scale_ext);
		new_samples = nullptr;
		state = ST_CAPTURING;
		Platform::leds[0].set();

		range_psd.reset();
		range_decimated.reset();
		range_in.reset();

		// Wait for new samples and then discard them. This is easier than
		// resetting DMA but...
		// TODO: Make setting source reset DMA (but this is still safe
		PT_WAIT_UNTIL(pt, new_samples);
		new_samples = nullptr;

		static sampleFractional window_scale = 1.0;// / (float)num_windows;

		while(state == ST_CAPTURING){
			PT_WAIT_UNTIL(pt, new_samples || pending_reset);
			if(pending_reset){ do_reset(); break;}

			num_samples += num_received;
			old_new_samples = new_samples;

			do_decimate(old_new_samples,
						&decimated_frame_buffer[decimation_write_head],
						num_received);
			decimation_write_head += num_received / 3;
			static_assert(decimated_frame_buffer_size == 768, "Decimation size has changed");
			if(decimation_write_head >= decimated_frame_buffer_size)
				decimation_write_head -= decimated_frame_buffer_size;
			num_samples += num_received;

			if(debug and window_count){
				// Quick check for overrun
				if(new_samples != old_new_samples)
					while(true);
			}

			new_samples = nullptr;

			num_decimated += num_received / 3;

			PT_YIELD(pt);

			// While there is a full transform available...
			while(num_decimated >= transform_len){
				// THIS IS RUN ONCE PER EPOCH
				constants = mk_multipliers();

				// The number of samples remaining before the end of the decimated_frame_buffer
				num_before_end = decimated_frame_buffer_size - decimation_read_head;
				if(num_before_end < transform_len){
					// Split in two
					arm_mult_q31((q31_t*)&decimated_frame_buffer[decimation_read_head],
								 (q31_t*)hamming512, (q31_t*)transform_buffer,
								 num_before_end);
					arm_mult_q31((q31_t*)&decimated_frame_buffer,
								 (q31_t*)&hamming512[num_before_end],
								 (q31_t*)&transform_buffer[num_before_end],
								 transform_len - num_before_end);

					weighted_vector_sum(
						constants.one_over,
						&decimated_frame_buffer[decimation_read_head],
						constants.one_minus,
						average_buffer,
						average_buffer,
						num_before_end
					);

					weighted_vector_sum(
						constants.one_over,
						decimated_frame_buffer,
						constants.one_minus,
						&average_buffer[num_before_end],
						&average_buffer[num_before_end],
						transform_len - num_before_end
					);

				} else {
					// All in one shot
					arm_mult_q31(
						(q31_t*)&decimated_frame_buffer[decimation_read_head],
						(q31_t*)hamming512,
						(q31_t*)transform_buffer,
						transform_len
					);

					weighted_vector_sum(
						constants.one_over,
						&decimated_frame_buffer[decimation_read_head],
						constants.one_minus,
						average_buffer,
						average_buffer,
						transform_len
					);
				}

				for(uint32_t i = 0; i < transform_len; i++){
					if(transform_buffer[i].i > in_max)
						in_max = transform_buffer[i].i;
					if(transform_buffer[i].i < in_min)
						in_min = transform_buffer[i].i;
				}

				if(APulseController::do_buffer_dumps){
					APulseController::waveform_dump.copy_data((uint8_t const *)transform_buffer);
				}

				num_decimated -= (transform_len - overlap);

				// Advance decimation read pointer
				decimation_read_head += transform_len - overlap;
				if(decimation_read_head >= decimated_frame_buffer_size)
					decimation_read_head -= decimated_frame_buffer_size;

				arm_shift_q31((q31_t *)transform_buffer, 6, (q31_t *)transform_buffer, transform_len);

				for(uint32_t i = 0; i < transform_len; i++){
					range_decimated.check(transform_buffer[i]);
				}

				PT_YIELD(pt);

				arm_rfft_q31(&rfft, (q31_t*)transform_buffer,
							 (q31_t*)complex_transform);

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
				if(++window_count >= num_windows){
					if(calibrate_mic){
						for(uint32_t i = 0; i < transform_len / 2; i++){
							mag_psd[i] = mag_psd[i] * APulseController::coeffs[i / 16];
						}
						mag_psd[transform_len / 2] = mag_psd[transform_len / 2] * APulseController::coeffs[15];
					}
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
	new_samples = nullptr;
	num_samples = 0;
	num_received = 0;
	num_decimated = 0;
	decimation_write_head = 0;
	decimation_read_head = 0;
	start_time_ms = -1;
	window_count = 0;

	max = 0;
	min = 0;
	Platform::leds[0].clear();

	// Decimated frame buffer really doesn't need to be zero'd...
	for(auto &a : decimated_frame_buffer) a.setInternal(0);

	// These ones are important though...
	for(auto &a : mag_psd) a.setInternal(0);
	for(auto &a : average_buffer) a.setInternal(0);

	if(use_iir) {
		arm_biquad_cascade_df1_init_q31(&biquad_cascade, biquad_stages,
		                                (q31_t *)biquad_coeffs, biquad_state,
		                                biquad_shift);
	} else {
		auto result =
		arm_fir_decimate_init_q31(&decimate, decimate_fir_order, 3,
		                          (q31_t*)decimate_coeffs,
		                          (q31_t*)decimate_buffer,
		                          decimate_block_size);
		
		if(result != ARM_MATH_SUCCESS){
			while(true);
		}
	}
	

	is_reset = true;
	pending_reset = false;

	state = ST_RESET;
}


template <size_t n_out, size_t factor>
static void biquad_cascade_q31_decimate(arm_biquad_casd_df1_inst_q31 &inst,
                                        q31_t * in, q31_t * out){
	static q31_t tmp[n_out * factor];
	auto tmpiter = tmp;
	arm_biquad_cascade_df1_q31(&inst, in, tmp, n_out * factor);
	for(auto i = n_out; i; i--){
 		*(out++) = *tmpiter;
 		tmpiter += factor;
	}
}


void InputDSP::do_decimate(sampleFractional const * src,
						   sampleFractional * dst,
						   size_t n_in){
	static sampleFractional const * iter_in;
	iter_in = src;
	for(auto i = 0; i < (n_in / decimate_block_size); i++){
		arm_shift_q31((q31_t*)iter_in, -6, (q31_t*)iter_in, decimate_block_size);
		if(use_iir) {
			biquad_cascade_q31_decimate<decimate_block_size / 3, 3>(
			            biquad_cascade, (q31_t *)iter_in, (q31_t *)dst);
		} else {
			arm_fir_decimate_q31(&decimate, (q31_t*)iter_in,
			            (q31_t*)dst, decimate_block_size);
		}
		iter_in += decimate_block_size;
		dst += decimate_block_size / 3;
	}
}

void InputDSP::put_samplesI(sample_t * samples, size_t n){
	if(state == ST_CAPTURING){
		new_samples = reinterpret_cast<sampleFractional *>(samples);
		num_received = n;
		for(uint32_t i = 0; i < n; i++){
			range_in.check(new_samples[i]);
		}
	}
}

// np.round(signal.windows.hamming(512) * 2**31).astype(np.int32)
//InputDSP::sample_t const InputDSP::hamming512[512] = {
//	0.08      ,  0.08003477,  0.08013909,  0.08031292,  0.08055626,
//	0.08086906,  0.08125127,  0.08170284,  0.0822237 ,  0.08281376,
//	0.08347295,  0.08420116,  0.08499828,  0.08586418,  0.08679875,
//	0.08780184,  0.08887329,  0.09001294,  0.09122063,  0.09249617,
//	0.09383936,  0.09525001,  0.09672789,  0.0982728 ,  0.09988448,
//	0.10156271,  0.10330722,  0.10511775,  0.10699403,  0.10893578,
//	0.1109427 ,  0.11301448,  0.11515082,  0.11735139,  0.11961586,
//	0.12194389,  0.12433512,  0.12678919,  0.12930573,  0.13188437,
//	0.13452471,  0.13722635,  0.13998888,  0.14281189,  0.14569495,
//	0.14863762,  0.15163946,  0.15470002,  0.15781883,  0.16099542,
//	0.16422931,  0.16752001,  0.17086702,  0.17426984,  0.17772796,
//	0.18124085,  0.18480797,  0.1884288 ,  0.19210278,  0.19582935,
//	0.19960796,  0.20343803,  0.20731899,  0.21125024,  0.2152312 ,
//	0.21926125,  0.2233398 ,  0.22746622,  0.2316399 ,  0.23586019,
//	0.24012646,  0.24443807,  0.24879437,  0.25319469,  0.25763837,
//	0.26212475,  0.26665313,  0.27122284,  0.27583319,  0.28048347,
//	0.28517299,  0.28990103,  0.29466689,  0.29946984,  0.30430915,
//	0.3091841 ,  0.31409394,  0.31903793,  0.32401534,  0.32902539,
//	0.33406735,  0.33914043,  0.34424389,  0.34937694,  0.35453881,
//	0.35972872,  0.36494588,  0.37018951,  0.37545881,  0.38075299,
//	0.38607124,  0.39141277,  0.39677676,  0.4021624 ,  0.40756889,
//	0.41299539,  0.4184411 ,  0.42390518,  0.42938682,  0.43488518,
//	0.44039943,  0.44592874,  0.45147227,  0.45702919,  0.46259865,
//	0.46817981,  0.47377183,  0.47937386,  0.48498506,  0.49060458,
//	0.49623157,  0.50186517,  0.50750453,  0.51314881,  0.51879715,
//	0.5244487 ,  0.5301026 ,  0.53575799,  0.54141402,  0.54706984,
//	0.55272459,  0.55837742,  0.56402747,  0.56967389,  0.57531582,
//	0.58095241,  0.58658281,  0.59220616,  0.59782163,  0.60342835,
//	0.60902548,  0.61461218,  0.6201876 ,  0.62575089,  0.63130122,
//	0.63683774,  0.64235963,  0.64786604,  0.65335614,  0.6588291 ,
//	0.6642841 ,  0.66972031,  0.67513691,  0.68053307,  0.68590799,
//	0.69126085,  0.69659084,  0.70189716,  0.707179  ,  0.71243557,
//	0.71766606,  0.72286969,  0.72804568,  0.73319324,  0.73831159,
//	0.74339995,  0.74845757,  0.75348367,  0.75847749,  0.76343829,
//	0.7683653 ,  0.77325778,  0.77811501,  0.78293623,  0.78772072,
//	0.79246776,  0.79717663,  0.80184662,  0.80647702,  0.81106714,
//	0.81561627,  0.82012373,  0.82458885,  0.82901093,  0.83338932,
//	0.83772336,  0.84201238,  0.84625575,  0.85045281,  0.85460293,
//	0.8587055 ,  0.86275987,  0.86676546,  0.87072163,  0.87462781,
//	0.8784834 ,  0.88228781,  0.88604048,  0.88974082,  0.89338829,
//	0.89698234,  0.90052241,  0.90400798,  0.90743851,  0.91081349,
//	0.91413241,  0.91739477,  0.92060007,  0.92374783,  0.92683757,
//	0.92986882,  0.93284114,  0.93575406,  0.93860715,  0.94139997,
//	0.94413211,  0.94680315,  0.94941269,  0.95196032,  0.95444568,
//	0.95686838,  0.95922805,  0.96152434,  0.9637569 ,  0.9659254 ,
//	0.9680295 ,  0.97006889,  0.97204326,  0.97395231,  0.97579575,
//	0.97757331,  0.97928471,  0.98092969,  0.98250802,  0.98401944,
//	0.98546374,  0.98684068,  0.98815007,  0.98939171,  0.9905654 ,
//	0.99167097,  0.99270826,  0.99367711,  0.99457736,  0.99540889,
//	0.99617157,  0.99686528,  0.99748992,  0.99804539,  0.99853161,
//	0.99894851,  0.99929602,  0.99957409,  0.99978268,  0.99992176,
//	0.99999131,  0.99999131,  0.99992176,  0.99978268,  0.99957409,
//	0.99929602,  0.99894851,  0.99853161,  0.99804539,  0.99748992,
//	0.99686528,  0.99617157,  0.99540889,  0.99457736,  0.99367711,
//	0.99270826,  0.99167097,  0.9905654 ,  0.98939171,  0.98815007,
//	0.98684068,  0.98546374,  0.98401944,  0.98250802,  0.98092969,
//	0.97928471,  0.97757331,  0.97579575,  0.97395231,  0.97204326,
//	0.97006889,  0.9680295 ,  0.9659254 ,  0.9637569 ,  0.96152434,
//	0.95922805,  0.95686838,  0.95444568,  0.95196032,  0.94941269,
//	0.94680315,  0.94413211,  0.94139997,  0.93860715,  0.93575406,
//	0.93284114,  0.92986882,  0.92683757,  0.92374783,  0.92060007,
//	0.91739477,  0.91413241,  0.91081349,  0.90743851,  0.90400798,
//	0.90052241,  0.89698234,  0.89338829,  0.88974082,  0.88604048,
//	0.88228781,  0.8784834 ,  0.87462781,  0.87072163,  0.86676546,
//	0.86275987,  0.8587055 ,  0.85460293,  0.85045281,  0.84625575,
//	0.84201238,  0.83772336,  0.83338932,  0.82901093,  0.82458885,
//	0.82012373,  0.81561627,  0.81106714,  0.80647702,  0.80184662,
//	0.79717663,  0.79246776,  0.78772072,  0.78293623,  0.77811501,
//	0.77325778,  0.7683653 ,  0.76343829,  0.75847749,  0.75348367,
//	0.74845757,  0.74339995,  0.73831159,  0.73319324,  0.72804568,
//	0.72286969,  0.71766606,  0.71243557,  0.707179  ,  0.70189716,
//	0.69659084,  0.69126085,  0.68590799,  0.68053307,  0.67513691,
//	0.66972031,  0.6642841 ,  0.6588291 ,  0.65335614,  0.64786604,
//	0.64235963,  0.63683774,  0.63130122,  0.62575089,  0.6201876 ,
//	0.61461218,  0.60902548,  0.60342835,  0.59782163,  0.59220616,
//	0.58658281,  0.58095241,  0.57531582,  0.56967389,  0.56402747,
//	0.55837742,  0.55272459,  0.54706984,  0.54141402,  0.53575799,
//	0.5301026 ,  0.5244487 ,  0.51879715,  0.51314881,  0.50750453,
//	0.50186517,  0.49623157,  0.49060458,  0.48498506,  0.47937386,
//	0.47377183,  0.46817981,  0.46259865,  0.45702919,  0.45147227,
//	0.44592874,  0.44039943,  0.43488518,  0.42938682,  0.42390518,
//	0.4184411 ,  0.41299539,  0.40756889,  0.4021624 ,  0.39677676,
//	0.39141277,  0.38607124,  0.38075299,  0.37545881,  0.37018951,
//	0.36494588,  0.35972872,  0.35453881,  0.34937694,  0.34424389,
//	0.33914043,  0.33406735,  0.32902539,  0.32401534,  0.31903793,
//	0.31409394,  0.3091841 ,  0.30430915,  0.29946984,  0.29466689,
//	0.28990103,  0.28517299,  0.28048347,  0.27583319,  0.27122284,
//	0.26665313,  0.26212475,  0.25763837,  0.25319469,  0.24879437,
//	0.24443807,  0.24012646,  0.23586019,  0.2316399 ,  0.22746622,
//	0.2233398 ,  0.21926125,  0.2152312 ,  0.21125024,  0.20731899,
//	0.20343803,  0.19960796,  0.19582935,  0.19210278,  0.1884288 ,
//	0.18480797,  0.18124085,  0.17772796,  0.17426984,  0.17086702,
//	0.16752001,  0.16422931,  0.16099542,  0.15781883,  0.15470002,
//	0.15163946,  0.14863762,  0.14569495,  0.14281189,  0.13998888,
//	0.13722635,  0.13452471,  0.13188437,  0.12930573,  0.12678919,
//	0.12433512,  0.12194389,  0.11961586,  0.11735139,  0.11515082,
//	0.11301448,  0.1109427 ,  0.10893578,  0.10699403,  0.10511775,
//	0.10330722,  0.10156271,  0.09988448,  0.0982728 ,  0.09672789,
//	0.09525001,  0.09383936,  0.09249617,  0.09122063,  0.09001294,
//	0.08887329,  0.08780184,  0.08679875,  0.08586418,  0.08499828,
//	0.08420116,  0.08347295,  0.08281376,  0.0822237 ,  0.08170284,
//	0.08125127,  0.08086906,  0.08055626,  0.08031292,  0.08013909,
//	0.08003477,  0.08
//};

// Nuttall
InputDSP::sample_t const InputDSP::hamming512[512] = {
        0.0003628 ,  0.0003657 ,  0.00037442,  0.00038899,  0.00040945,
0.00043586,  0.00046832,  0.00050693,  0.0005518 ,  0.00060309,
0.00066096,  0.00072557,  0.00079714,  0.00087587,  0.00096201,
0.00105581,  0.00115754,  0.00126749,  0.00138598,  0.00151334,
0.00164991,  0.00179605,  0.00195216,  0.00211864,  0.00229591,
0.00248441,  0.00268459,  0.00289694,  0.00312195,  0.00336013,
0.00361201,  0.00387814,  0.00415908,  0.00445542,  0.00476776,
0.00509671,  0.00544291,  0.00580701,  0.00618967,  0.00659159,
0.00701347,  0.00745601,  0.00791995,  0.00840605,  0.00891506,
0.00944776,  0.01000494,  0.01058742,  0.01119601,  0.01183156,
0.01249491,  0.01318692,  0.01390847,  0.01466045,  0.01544375,
0.01625929,  0.017108  ,  0.0179908 ,  0.01890864,  0.01986248,
0.02085327,  0.02188199,  0.02294962,  0.02405715,  0.02520557,
0.02639588,  0.0276291 ,  0.02890623,  0.03022829,  0.03159631,
0.0330113 ,  0.03447431,  0.03598635,  0.03754846,  0.03916167,
0.04082702,  0.04254553,  0.04431822,  0.04614614,  0.0480303 ,
0.04997171,  0.0519714 ,  0.05403036,  0.05614961,  0.05833013,
0.0605729 ,  0.06287891,  0.06524912,  0.06768447,  0.07018591,
0.07275436,  0.07539075,  0.07809596,  0.08087088,  0.08371637,
0.08663328,  0.08962244,  0.09268464,  0.09582068,  0.09903132,
0.1023173 ,  0.10567933,  0.1091181 ,  0.11263429,  0.11622852,
0.1199014 ,  0.12365351,  0.1274854 ,  0.13139759,  0.13539056,
0.13946476,  0.14362061,  0.1478585 ,  0.15217877,  0.15658172,
0.16106763,  0.16563674,  0.17028923,  0.17502525,  0.17984491,
0.18474829,  0.1897354 ,  0.19480623,  0.19996071,  0.20519874,
0.21052014,  0.21592473,  0.22141225,  0.2269824 ,  0.23263483,
0.23836915,  0.2441849 ,  0.25008159,  0.25605867,  0.26211553,
0.26825153,  0.27446595,  0.28075804,  0.28712698,  0.29357192,
0.30009192,  0.30668602,  0.31335318,  0.32009233,  0.32690234,
0.333782  ,  0.34073007,  0.34774526,  0.35482621,  0.36197151,
0.3691797 ,  0.37644926,  0.38377862,  0.39116616,  0.3986102 ,
0.40610901,  0.41366082,  0.42126378,  0.42891601,  0.43661558,
0.4443605 ,  0.45214873,  0.45997819,  0.46784675,  0.47575223,
0.48369239,  0.49166498,  0.49966767,  0.5076981 ,  0.51575387,
0.52383253,  0.5319316 ,  0.54004855,  0.54818082,  0.55632581,
0.56448088,  0.57264336,  0.58081054,  0.58897969,  0.59714804,
0.60531279,  0.61347112,  0.62162018,  0.6297571 ,  0.63787897,
0.64598287,  0.65406588,  0.66212502,  0.67015733,  0.67815981,
0.68612946,  0.69406328,  0.70195823,  0.70981129,  0.71761942,
0.72537958,  0.73308872,  0.7407438 ,  0.74834178,  0.75587962,
0.76335428,  0.77076275,  0.77810199,  0.78536901,  0.79256082,
0.79967442,  0.80670686,  0.8136552 ,  0.82051652,  0.8272879 ,
0.83396648,  0.84054941,  0.84703386,  0.85341705,  0.8596962 ,
0.86586861,  0.87193157,  0.87788243,  0.88371858,  0.88943745,
0.8950365 ,  0.90051326,  0.90586528,  0.91109017,  0.91618559,
0.92114924,  0.9259789 ,  0.93067237,  0.93522753,  0.93964231,
0.9439147 ,  0.94804275,  0.95202456,  0.95585831,  0.95954224,
0.96307464,  0.9664539 ,  0.96967845,  0.97274679,  0.9756575 ,
0.97840922,  0.98100069,  0.98343069,  0.98569808,  0.9878018 ,
0.98974089,  0.99151441,  0.99312155,  0.99456155,  0.99583374,
0.99693751,  0.99787236,  0.99863783,  0.99923357,  0.9996593 ,
0.99991481,  1.        ,  0.99991481,  0.9996593 ,  0.99923357,
0.99863783,  0.99787236,  0.99693751,  0.99583374,  0.99456155,
0.99312155,  0.99151441,  0.98974089,  0.9878018 ,  0.98569808,
0.98343069,  0.98100069,  0.97840922,  0.9756575 ,  0.97274679,
0.96967845,  0.9664539 ,  0.96307464,  0.95954224,  0.95585831,
0.95202456,  0.94804275,  0.9439147 ,  0.93964231,  0.93522753,
0.93067237,  0.9259789 ,  0.92114924,  0.91618559,  0.91109017,
0.90586528,  0.90051326,  0.8950365 ,  0.88943745,  0.88371858,
0.87788243,  0.87193157,  0.86586861,  0.8596962 ,  0.85341705,
0.84703386,  0.84054941,  0.83396648,  0.8272879 ,  0.82051652,
0.8136552 ,  0.80670686,  0.79967442,  0.79256082,  0.78536901,
0.77810199,  0.77076275,  0.76335428,  0.75587962,  0.74834178,
0.7407438 ,  0.73308872,  0.72537958,  0.71761942,  0.70981129,
0.70195823,  0.69406328,  0.68612946,  0.67815981,  0.67015733,
0.66212502,  0.65406588,  0.64598287,  0.63787897,  0.6297571 ,
0.62162018,  0.61347112,  0.60531279,  0.59714804,  0.58897969,
0.58081054,  0.57264336,  0.56448088,  0.55632581,  0.54818082,
0.54004855,  0.5319316 ,  0.52383253,  0.51575387,  0.5076981 ,
0.49966767,  0.49166498,  0.48369239,  0.47575223,  0.46784675,
0.45997819,  0.45214873,  0.4443605 ,  0.43661558,  0.42891601,
0.42126378,  0.41366082,  0.40610901,  0.3986102 ,  0.39116616,
0.38377862,  0.37644926,  0.3691797 ,  0.36197151,  0.35482621,
0.34774526,  0.34073007,  0.333782  ,  0.32690234,  0.32009233,
0.31335318,  0.30668602,  0.30009192,  0.29357192,  0.28712698,
0.28075804,  0.27446595,  0.26825153,  0.26211553,  0.25605867,
0.25008159,  0.2441849 ,  0.23836915,  0.23263483,  0.2269824 ,
0.22141225,  0.21592473,  0.21052014,  0.20519874,  0.19996071,
0.19480623,  0.1897354 ,  0.18474829,  0.17984491,  0.17502525,
0.17028923,  0.16563674,  0.16106763,  0.15658172,  0.15217877,
0.1478585 ,  0.14362061,  0.13946476,  0.13539056,  0.13139759,
0.1274854 ,  0.12365351,  0.1199014 ,  0.11622852,  0.11263429,
0.1091181 ,  0.10567933,  0.1023173 ,  0.09903132,  0.09582068,
0.09268464,  0.08962244,  0.08663328,  0.08371637,  0.08087088,
0.07809596,  0.07539075,  0.07275436,  0.07018591,  0.06768447,
0.06524912,  0.06287891,  0.0605729 ,  0.05833013,  0.05614961,
0.05403036,  0.0519714 ,  0.04997171,  0.0480303 ,  0.04614614,
0.04431822,  0.04254553,  0.04082702,  0.03916167,  0.03754846,
0.03598635,  0.03447431,  0.0330113 ,  0.03159631,  0.03022829,
0.02890623,  0.0276291 ,  0.02639588,  0.02520557,  0.02405715,
0.02294962,  0.02188199,  0.02085327,  0.01986248,  0.01890864,
0.0179908 ,  0.017108  ,  0.01625929,  0.01544375,  0.01466045,
0.01390847,  0.01318692,  0.01249491,  0.01183156,  0.01119601,
0.01058742,  0.01000494,  0.00944776,  0.00891506,  0.00840605,
0.00791995,  0.00745601,  0.00701347,  0.00659159,  0.00618967,
0.00580701,  0.00544291,  0.00509671,  0.00476776,  0.00445542,
0.00415908,  0.00387814,  0.00361201,  0.00336013,  0.00312195,
0.00289694,  0.00268459,  0.00248441,  0.00229591,  0.00211864,
0.00195216,  0.00179605,  0.00164991,  0.00151334,  0.00138598,
0.00126749,  0.00115754,  0.00105581,  0.00096201,  0.00087587,
0.00079714,  0.00072557,  0.00066096,  0.00060309,  0.0005518 ,
0.00050693,  0.00046832,  0.00043586,  0.00040945,  0.00038899,
0.00037442,  0.0003657};

//InputDSP::sample_t const InputDSP::hamming512[512] = {

