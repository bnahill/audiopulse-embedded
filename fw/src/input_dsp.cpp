#include <input_dsp.h>
#include <arm_math.h>

InputDSP::sample_t const * InputDSP::new_samples  = nullptr;
uint32_t InputDSP::num_samples                    = 0;

arm_fir_decimate_instance_q31 InputDSP::decimate;


/**
 * Generated coefficients from http://t-filter.appspot.com/fir/index.html
 * Passband 0-6kHz 1dB ripple
 * Stopband 8kHz-24kHz 15dB attenuation
 */
InputDSP::sample_t const InputDSP::decimate_coeffs[decimate_fir_order] = {
	-212529062, 246916705, 175198082, 50163381, -128851454, -254415550,
	-182451133, 148530534, 648372557, 1100657478, 1282504819, 1100657478,
	648372557, 148530534, -182451133, -254415550, -128851454, 50163381,
	175198082, 246916705, -212529062
};

PT_THREAD(InputDSP::pt_dsp(struct pt * pt)){
	PT_BEGIN(pt);
	
	static sample_t frame_buffer[512];
	static uint32_t theta = 0;
	static uint32_t buffer_sel = 0;
	
	////////////////////////////
	// Configure CODEC
	////////////////////////////
	
	AK4621::set_channels(AK4621::CH_MIC);
	AK4621::set_in_cb(put_samplesI);
	
	////////////////////////////
	// Prepare FIR decimation
	////////////////////////////
	static_assert(AK4621::in_buffer_size == 768, "Wrong buffer size for DSP");
	static sample_t decimate_buffer[decimate_block_size +
	                                        decimate_fir_order - 1];	
	arm_fir_decimate_init_q31(&decimate, decimate_fir_order, 3,
	                          (q31_t*)decimate_coeffs,
	                          decimate_buffer,
	                          decimate_block_size);

	while(true){
		PT_WAIT_UNTIL(pt, new_samples);
		
		do_decimate(&frame_buffer[buffer_sel * 256]);
		buffer_sel ^= 1;
		num_samples += 256;
		
		
		
	}

	PT_END(pt);
}

void InputDSP::do_decimate(sample_t * dst){
	static sample_t const * iter_in;
	iter_in = new_samples;
	for(auto i = 0; i < (768 / decimate_block_size); i++){
		arm_fir_decimate_fast_q31(&decimate, (q31_t*)iter_in,
		                          dst, decimate_block_size);
		iter_in += decimate_block_size;
		dst += decimate_block_size / 3;
	}
}


void InputDSP::put_samplesI(sample_t * samples){
	new_samples = samples;
	num_samples += AK4621::in_buffer_size;
}
