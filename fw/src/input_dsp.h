#ifndef __APULSE_INPUT_DSP_H_
#define __APULSE_INPUT_DSP_H_

#include <driver/platform.h>
#include <driver/codec.h>
#include <arm_math.h>

class InputDSP {
public:
	static PT_THREAD(pt_dsp(struct pt * pt));
	
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
	
	static constexpr uint16_t decimate_fir_order = 21;
	static AK4621::sample_t const decimate_coeffs[decimate_fir_order];
	static constexpr uint16_t decimate_block_size = 48;
	
	static void do_decimate(sample_t * dst);
	static arm_fir_decimate_instance_q31 decimate;
};

#endif // __APULSE_INPUT_DSP_H_