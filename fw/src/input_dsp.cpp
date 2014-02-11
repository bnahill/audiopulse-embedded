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

InputDSP::sampleFractional const * InputDSP::new_samples  = nullptr;
uint32_t InputDSP::num_samples                    = 0;

__attribute__((aligned(512)))
__attribute__((section(".m_data2")))
InputDSP::sampleFractional InputDSP::decimated_frame_buffer[decimated_frame_buffer_size];

uint16_t InputDSP::num_decimated;

arm_fir_decimate_instance_q31 InputDSP::decimate;

decltype(InputDSP::start_time_ms) InputDSP::start_time_ms = -1;
decltype(InputDSP::num_windows) InputDSP::num_windows;

bool InputDSP::is_reset, InputDSP::pending_reset;

uint32_t InputDSP::decimated_iter = 0;
uint8_t InputDSP::buffer_sel = 0;
uint32_t InputDSP::num_before_end = 0;


arm_rfft_instance_q31 InputDSP::rfft;
arm_cfft_radix4_instance_q31 InputDSP::cfft;

__attribute__((section(".m_data2")))
InputDSP::sampleFractional InputDSP::transform_buffer[transform_len];

__attribute__((section(".m_data2")))
decltype(InputDSP::complex_transform) InputDSP::complex_transform;

__attribute__((section(".m_data2")))
decltype(InputDSP::mag_psd) InputDSP::mag_psd;

// __attribute__((section(".m_data2")))
// decltype(InputDSP::mag_psd) InputDSP::mag_psd;

__attribute__((section(".m_data2")))
InputDSP::sampleFractional InputDSP::average_buffer[transform_len];

uint32_t InputDSP::window_count;

__attribute__((section(".m_data2")))
InputDSP::sampleFractional InputDSP::decimate_buffer[decimate_block_size +
                                                     decimate_fir_order - 1];

decltype(InputDSP::overlap) InputDSP::overlap;

decltype(InputDSP::state) InputDSP::state = ST_UNKNOWN;

AK4621::Src InputDSP::src;
InputDSP::sampleFractional InputDSP::scale_mic;
InputDSP::sampleFractional InputDSP::scale_ext;

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
/**
 * Generated coefficients from http://t-filter.appspot.com/fir/index.html
 * Passband 0-6kHz -0.5dB with 1dB ripple (max 0dB to avoid saturation)
 * Stopband 8kHz-24kHz 40dB attenuation
 */
InputDSP::sample_t const InputDSP::decimate_coeffs[decimate_fir_order] = {
	-29719891, -22214914, -4305276, 28510937, 55098416, 51210097, 10347956,
	-44183282, -69791987, -35719183, 46385596, 119099633, 113845418, 5597554,
	-150985522, -237046029, -136950297, 180834943, 623954165, 1010548673,
	1163721112, 1010548673, 623954165, 180834943, -136950297, -237046029,
	-150985522, 5597554, 113845418, 119099633, 46385596, -35719183, -69791987,
	-44183282, 10347956, 51210097, 55098416, 28510937, -4305276, -22214914,
	-29719891
};



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
		
		// Wait for new samples and then discard them. This is easier than
		// resetting DMA but...
		// TODO: Make setting source reset DMA
		PT_WAIT_UNTIL(pt, new_samples);
		new_samples = nullptr;

		while(state == ST_CAPTURING){
			PT_WAIT_UNTIL(pt, new_samples || pending_reset);
			if(pending_reset){ do_reset(); break;}

			old_new_samples = new_samples;
		
			do_decimate(&decimated_frame_buffer[buffer_sel * decimate_output_size]);
			buffer_sel = (buffer_sel + 1) % 3;
			num_samples += 256;

			if(debug){
				// Quick check for overrun
				if(new_samples != old_new_samples)
					while(true);
			}
			
			new_samples = nullptr;

			num_decimated += 256;

			PT_YIELD(pt);

			// While there is a full transform available...
			while(num_decimated >= transform_len){
				// THIS IS RUN ONCE PER EPOCH
				constants = mk_multipliers();

				// The number of samples remaining before the end of the decimated_frame_buffer
				num_before_end = decimated_frame_buffer_size - decimated_iter;
				if(num_before_end < transform_len){
					// Split in two
					arm_mult_q31((q31_t*)&decimated_frame_buffer[decimated_iter],
					             (q31_t*)hamming512, (q31_t*)transform_buffer,
					             num_before_end);
					arm_mult_q31((q31_t*)&decimated_frame_buffer,
					             (q31_t*)&hamming512[num_before_end],
					             (q31_t*)&transform_buffer[num_before_end],
					             transform_len - num_before_end);

					weighted_vector_sum(
						constants.one_over,
						&decimated_frame_buffer[decimated_iter],
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
						(q31_t*)decimated_frame_buffer,
						(q31_t*)hamming512,
						(q31_t*)transform_buffer,
						transform_len
					);

					weighted_vector_sum(
						constants.one_over,
						&decimated_frame_buffer[decimated_iter],
						constants.one_minus,
						average_buffer,
						average_buffer,
						transform_len
					);
				}

				num_decimated -= (transform_len - overlap);
				decimated_iter = ( decimated_iter + (transform_len - overlap)) &
						(decimated_frame_buffer_size - 1);

				PT_YIELD(pt);

				arm_rfft_q31(&rfft, (q31_t*)transform_buffer,
				             (q31_t*)complex_transform);

				PT_YIELD(pt);


				complex_power_avg_update(
					(powerFractional)constants.one_over,
					complex_transform,
					(powerFractional)constants.one_minus,
					mag_psd,
					mag_psd,
					transform_len + 2
				);

				// Check if we've processed enough windows to shut down
				if(++window_count >= num_windows){
					for(uint32_t i = 0; i < transform_len / 2; i++){
						mag_psd[i] = mag_psd[i] * APulseController::coeffs[i / 16];
					}
					mag_psd[transform_len / 2] = mag_psd[transform_len / 2] * APulseController::coeffs[15];
					state = ST_DONE;
					break;
				}

				PT_YIELD(pt);

			}
		}
	}

	PT_END(pt);
}



void InputDSP::do_decimate(sampleFractional * dst){
	static sampleFractional const * iter_in;
	iter_in = new_samples;
	for(auto i = 0; i < (768 / decimate_block_size); i++){
		arm_fir_decimate_fast_q31(&decimate, (q31_t*)iter_in,
		                          (q31_t*)dst, decimate_block_size);
		iter_in += decimate_block_size;
		dst += decimate_block_size / 3;
	}
}

void InputDSP::put_samplesI(sample_t * samples, size_t n){
	if(state == ST_CAPTURING){
		new_samples = reinterpret_cast<sampleFractional *>(samples);
		num_samples = n;
	}
}

// np.round(signal.windows.hamming(512) * 2**31).astype(np.int32)
InputDSP::sample_t const InputDSP::hamming512[512] = {
	171798692,  171873366,  172097377,  172470691,  172993252,
	173664981,  174485775,  175455512,  176574044,  177841202,
	179256795,  180820609,  182532407,  184391930,  186398898,
	188553007,  190853931,  193301322,  195894811,  198634005,
	201518490,  204547830,  207721567,  211039221,  214500291,
	218104253,  221850563,  225738654,  229767938,  233937807,
	238247629,  242696753,  247284506,  252010195,  256873106,
	261872503,  267007630,  272277711,  277681950,  283219528,
	288889610,  294691337,  300623833,  306686200,  312877523,
	319196864,  325643270,  332215764,  338913354,  345735026,
	352679750,  359746476,  366934134,  374241640,  381667886,
	389211752,  396872096,  404647760,  412537569,  420540330,
	428654833,  436879851,  445214140,  453656441,  462205478,
	470859957,  479618570,  488479993,  497442887,  506505896,
	515667650,  524926765,  534281839,  543731459,  553274196,
	562908608,  572633237,  582446614,  592347255,  602333663,
	612404328,  622557728,  632792328,  643106580,  653498924,
	663967791,  674511597,  685128747,  695817638,  706576652,
	717404163,  728298535,  739258120,  750281260,  761366291,
	772511535,  783715308,  794975915,  806291655,  817660817,
	829081681,  840552522,  852071604,  863637187,  875247521,
	886900852,  898595417,  910329449,  922101174,  933908811,
	945750576,  957624678,  969529322,  981462709,  993423034,
	1005408488, 1017417261, 1029447536, 1041497494, 1053565314,
	1065649172, 1077747239, 1089857688, 1101978687, 1114108404,
	1126245005, 1138386655, 1150531518, 1162677759, 1174823540,
	1186967026, 1199106381, 1211239769, 1223365356, 1235481309,
	1247585796, 1259676987, 1271753054, 1283812171, 1295852516,
	1307872266, 1319869607, 1331842723, 1343789804, 1355709045,
	1367598643, 1379456801, 1391281725, 1403071629, 1414824729,
	1426539249, 1438213418, 1449845470, 1461433648, 1472976198,
	1484471377, 1495917446, 1507312674, 1518655339, 1529943727,
	1541176129, 1552350849, 1563466196, 1574520491, 1585512061,
	1596439246, 1607300392, 1618093859, 1628818014, 1639471236,
	1650051914, 1660558449, 1670989252, 1681342746, 1691617365,
	1701811558, 1711923782, 1721952508, 1731896220, 1741753415,
	1751522603, 1761202306, 1770791062, 1780287420, 1789689945,
	1798997215, 1808207823, 1817320377, 1826333499, 1835245826,
	1844056011, 1852762722, 1861364642, 1869860471, 1878248925,
	1886528735, 1894698650, 1902757434, 1910703869, 1918536754,
	1926254904, 1933857152, 1941342350, 1948709365, 1955957084,
	1963084411, 1970090268, 1976973596, 1983733355, 1990368523,
	1996878095, 2003261090, 2009516540, 2015643501, 2021641046,
	2027508268, 2033244281, 2038848217, 2044319228, 2049656489,
	2054859192, 2059926549, 2064857796, 2069652187, 2074308996,
	2078827520, 2083207076, 2087447001, 2091546655, 2095505418,
	2099322690, 2102997896, 2106530478, 2109919905, 2113165662,
	2116267259, 2119224227, 2122036120, 2124702512, 2127222999,
	2129597202, 2131824760, 2133905338, 2135838620, 2137624314,
	2139262151, 2140751883, 2142093284, 2143286151, 2144330305,
	2145225588, 2145971864, 2146569020, 2147016966, 2147315634,
	2147464979, 2147464979, 2147315634, 2147016966, 2146569020,
	2145971864, 2145225588, 2144330305, 2143286151, 2142093284,
	2140751883, 2139262151, 2137624314, 2135838620, 2133905338,
	2131824760, 2129597202, 2127222999, 2124702512, 2122036120,
	2119224227, 2116267259, 2113165662, 2109919905, 2106530478,
	2102997896, 2099322690, 2095505418, 2091546655, 2087447001,
	2083207076, 2078827520, 2074308996, 2069652187, 2064857796,
	2059926549, 2054859192, 2049656489, 2044319228, 2038848217,
	2033244281, 2027508268, 2021641046, 2015643501, 2009516540,
	2003261090, 1996878095, 1990368523, 1983733355, 1976973596,
	1970090268, 1963084411, 1955957084, 1948709365, 1941342350,
	1933857152, 1926254904, 1918536754, 1910703869, 1902757434,
	1894698650, 1886528735, 1878248925, 1869860471, 1861364642,
	1852762722, 1844056011, 1835245826, 1826333499, 1817320377,
	1808207823, 1798997215, 1789689945, 1780287420, 1770791062,
	1761202306, 1751522603, 1741753415, 1731896220, 1721952508,
	1711923782, 1701811558, 1691617365, 1681342746, 1670989252,
	1660558449, 1650051914, 1639471236, 1628818014, 1618093859,
	1607300392, 1596439246, 1585512061, 1574520491, 1563466196,
	1552350849, 1541176129, 1529943727, 1518655339, 1507312674,
	1495917446, 1484471377, 1472976198, 1461433648, 1449845470,
	1438213418, 1426539249, 1414824729, 1403071629, 1391281725,
	1379456801, 1367598643, 1355709045, 1343789804, 1331842723,
	1319869607, 1307872266, 1295852516, 1283812171, 1271753054,
	1259676987, 1247585796, 1235481309, 1223365356, 1211239769,
	1199106381, 1186967026, 1174823540, 1162677759, 1150531518,
	1138386655, 1126245005, 1114108404, 1101978687, 1089857688,
	1077747239, 1065649172, 1053565314, 1041497494, 1029447536,
	1017417261, 1005408488,  993423034,  981462709,  969529322,
	957624678,  945750576,  933908811,  922101174,  910329449,
	898595417,  886900852,  875247521,  863637187,  852071604,
	840552522,  829081681,  817660817,  806291655,  794975915,
	783715308,  772511535,  761366291,  750281260,  739258120,
	728298535,  717404163,  706576652,  695817638,  685128747,
	674511597,  663967791,  653498924,  643106580,  632792328,
	622557728,  612404328,  602333663,  592347255,  582446614,
	572633237,  562908608,  553274196,  543731459,  534281839,
	524926765,  515667650,  506505896,  497442887,  488479993,
	479618570,  470859957,  462205478,  453656441,  445214140,
	436879851,  428654833,  420540330,  412537569,  404647760,
	396872096,  389211752,  381667886,  374241640,  366934134,
	359746476,  352679750,  345735026,  338913354,  332215764,
	325643270,  319196864,  312877523,  306686200,  300623833,
	294691337,  288889610,  283219528,  277681950,  272277711,
	267007630,  261872503,  256873106,  252010195,  247284506,
	242696753,  238247629,  233937807,  229767938,  225738654,
	221850563,  218104253,  214500291,  211039221,  207721567,
	204547830,  201518490,  198634005,  195894811,  193301322,
	190853931,  188553007,  186398898,  184391930,  182532407,
	180820609,  179256795,  177841202,  176574044,  175455512,
	174485775,  173664981,  172993252,  172470691,  172097377,
	171873366,  171798692
};
