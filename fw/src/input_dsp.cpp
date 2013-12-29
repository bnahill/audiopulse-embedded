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

InputDSP::sample_t const * InputDSP::new_samples  = nullptr;
uint32_t InputDSP::num_samples                    = 0;

__attribute__((aligned(512)))
InputDSP::sample_t InputDSP::decimated_frame_buffer[512];

uint16_t InputDSP::num_decimated;

arm_fir_decimate_instance_q31 InputDSP::decimate;

/**
 * Generated coefficients from http://t-filter.appspot.com/fir/index.html
 * Passband 0-6kHz -0.5dB with 1dB ripple (max 0dB)
 * Stopband 8kHz-24kHz 15dB attenuation
 */
InputDSP::sample_t const InputDSP::decimate_coeffs[decimate_fir_order] = {
	-206371169, 232918033, 166081815, 48163641, -121174027, -240290533,
	-172723375, 139749183, 612254038, 1040027243, 1212012007, 1040027243,
	612254038, 139749183, -172723375, -240290533, -121174027, 48163641,
	166081815, 232918033, -206371169
};

InputDSP::sample_t const InputDSP::hamming256[256] = {
	171798691,  172098549,  172997940,  174496318,  176592773,
	179286034,  182574464,  186456067,  190928488,  195989010,
	201634561,  207861714,  214666689,  222045354,  229993230,
	238505492,  247576971,  257202161,  267375218,  278089966,
	289339900,  301118191,  313417688,  326230923,  339550119,
	353367189,  367673744,  382461100,  397720279,  413442017,
	429616769,  446234717,  463285770,  480759579,  498645533,
	516932775,  535610204,  554666478,  574090031,  593869069,
	613991586,  634445364,  655217986,  676296842,  697669134,
	719321887,  741241957,  763416035,  785830660,  808472224,
	831326981,  854381056,  877620453,  901031064,  924598677,
	948308982,  972147587,  996100018, 1020151734, 1044288134,
	1068494565, 1092756330, 1117058700, 1141386922, 1165726226,
	1190061836, 1214378978, 1238662889, 1262898826, 1287072075,
	1311167962, 1335171857, 1359069189, 1382845448, 1406486201,
	1429977096, 1453303870, 1476452363, 1499408522, 1522158408,
	1544688212, 1566984256, 1589033003, 1610821068, 1632335224,
	1653562408, 1674489736, 1695104500, 1715394187, 1735346479,
	1754949262, 1774190636, 1793058919, 1811542657, 1829630628,
	1847311852, 1864575593, 1881411371, 1897808965, 1913758421,
	1929250055, 1944274462, 1958822522, 1972885402, 1986454564,
	1999521772, 2012079091, 2024118898, 2035633885, 2046617060,
	2057061755, 2066961630, 2076310674, 2085103211, 2093333904,
	2100997757, 2108090115, 2114606673, 2120543476, 2125896919,
	2130663752, 2134841081, 2138426370, 2141417442, 2143812482,
	2145610035, 2146809010, 2147408680, 2147408680, 2146809010,
	2145610035, 2143812482, 2141417442, 2138426370, 2134841081,
	2130663752, 2125896919, 2120543476, 2114606673, 2108090115,
	2100997757, 2093333904, 2085103211, 2076310674, 2066961630,
	2057061755, 2046617060, 2035633885, 2024118898, 2012079091,
	1999521772, 1986454564, 1972885402, 1958822522, 1944274462,
	1929250055, 1913758421, 1897808965, 1881411371, 1864575593,
	1847311852, 1829630628, 1811542657, 1793058919, 1774190636,
	1754949262, 1735346479, 1715394187, 1695104500, 1674489736,
	1653562408, 1632335224, 1610821068, 1589033003, 1566984256,
	1544688212, 1522158408, 1499408522, 1476452363, 1453303870,
	1429977096, 1406486201, 1382845448, 1359069189, 1335171857,
	1311167962, 1287072075, 1262898826, 1238662889, 1214378978,
	1190061836, 1165726226, 1141386922, 1117058700, 1092756330,
	1068494565, 1044288134, 1020151734,  996100018,  972147587,
	948308982,  924598677,  901031064,  877620453,  854381056,
	831326981,  808472224,  785830660,  763416035,  741241957,
	719321887,  697669134,  676296842,  655217986,  634445364,
	613991586,  593869069,  574090031,  554666478,  535610204,
	516932775,  498645533,  480759579,  463285770,  446234717,
	429616769,  413442017,  397720279,  382461100,  367673744,
	353367189,  339550119,  326230923,  313417688,  301118191,
	289339900,  278089966,  267375218,  257202161,  247576971,
	238505492,  229993230,  222045354,  214666689,  207861714,
	201634561,  195989010,  190928488,  186456067,  182574464,
	179286034,  176592773,  174496318,  172997940,  172098549,
	171798691
};

PT_THREAD(InputDSP::pt_dsp(struct pt * pt)){
	PT_BEGIN(pt);
	
	static uint32_t theta = 0;
	static uint8_t buffer_sel = 0;
	static decltype(new_samples) old_new_samples;
	
	static sample_t decimate_buffer[decimate_block_size +
	                                decimate_fir_order - 1];
	
	////////////////////////////
	// Reset buffers
	////////////////////////////
	
	num_samples = 0;
	buffer_sel = 0;
	theta = 0;
	for(auto &a : decimated_frame_buffer) a = 0;
	for(auto &a : decimate_buffer) a = 0;
	
	////////////////////////////
	// Configure CODEC
	////////////////////////////
	
	AK4621::set_channels(AK4621::CH_MIC);
	AK4621::set_in_cb(put_samplesI);
	
	////////////////////////////
	// Prepare FIR decimation
	////////////////////////////
	static_assert(AK4621::in_buffer_size == 768, "Wrong buffer size for DSP");
	arm_fir_decimate_init_q31(&decimate, decimate_fir_order, 3,
	                          (q31_t*)decimate_coeffs,
	                          decimate_buffer,
	                          decimate_block_size);

	while(true){
		PT_WAIT_UNTIL(pt, new_samples);
		old_new_samples = new_samples;
		
		do_decimate(&decimated_frame_buffer[buffer_sel * 256]);
		buffer_sel ^= 1;
		num_samples += 256;
		
		if(debug){
			// Quick check for overrun
			if(new_samples != old_new_samples)
				while(true);
		}
			
		new_samples = nullptr;
		
		num_decimated += 256;
		
		PT_YIELD(pt);
		
		while(num_decimated >= 256){
			
		}
		
	}

	PT_END(pt);
}



void InputDSP::do_decimate(sample_t * dst 
){
	static sample_t const * iter_in;
	iter_in = new_samples;
	for(auto i = 0; i < (768 / decimate_block_size); i++){
		arm_fir_decimate_fast_q31(&decimate, (q31_t*)iter_in,
		                          dst, decimate_block_size);
		iter_in += decimate_block_size;
		dst += decimate_block_size / 3;
	}
}

// typedef struct {
// 	uint32_t fftLenReal;                        /**< length of the real FFT. */
// 	uint32_t fftLenBy2;                         /**< length of the complex FFT. */
// 	uint8_t ifftFlagR;                          /**< flag that selects forward (ifftFlagR=0) or inverse (ifftFlagR=1) transform. */
// 	uint8_t bitReverseFlagR;                        /**< flag that enables (bitReverseFlagR=1) or disables (bitReverseFlagR=0) bit reversal of output. */
// 	uint32_t twidCoefRModifier;                 /**< twiddle coefficient modifier that supports different size FFTs with the same twiddle factor table. */
// 	q31_t *pTwiddleAReal;                       /**< points to the real twiddle factor table. */
// 	q31_t *pTwiddleBReal;                       /**< points to the imag twiddle factor table. */
// 	arm_cfft_radix4_instance_q31 *pCfft;        /**< points to the complex FFT instance. */
// } arm_rfft_radix2_instance_q31;
// 
// static void arm_rfft_radix2_init(
// 	  arm_rfft_radix2_instance_q31 * S,
// 	  arm_cfft_radix2_instance_q31 * S_CFFT,
// 	  uint32_t fftLenReal,
// 	  uint32_t ifftFlagR,
// 	  uint32_t bitReverseFlag){
// }

void InputDSP::do_transform(){
	arm_rfft_instance_q31 fft;
	//arm_rfft_init_q31(&fft, )
	sample_t buffer;
}

void InputDSP::do_average(){
	
}


void InputDSP::put_samplesI(sample_t * samples){
	new_samples = samples;
	num_samples += AK4621::in_buffer_size;
}
