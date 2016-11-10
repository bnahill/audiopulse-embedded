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

#ifndef __APULSE_WAVEGEN_H_
#define __APULSE_WAVEGEN_H_

#include <driver/platform.h>
#include <driver/codec.h>
#include <arm_math.h>
#include <derivative.h>
#include <apulse_math.h>
#include <math.h>



class Generator {
public:
	Generator() :
		type(GEN_OFF), gain(0.0)
		{}
	typedef enum {
		GEN_OFF = 0,
		GEN_FIXED,
		GEN_CHIRP
	} gen_type_t;

	gen_type_t type;
	float f1;
	float f2;
	float w1;     //!< Start normalized angular rate
	float w2;     //!< End angular rate (chirp)
	uint16_t t1;     //!< The start time
	uint16_t t2;     //!< The finish time
	float theta;  //!< Current phase in wavetable
	uint16_t w_current; //!< Theta for angular rate in chirp, might not use
	uint8_t ch;      //!< Which channel is it?
    float gain;   //!< Gain applied to each full-scale sample

	inline operator bool() const {return type != GEN_OFF;}
	inline bool operator !() const {return type == GEN_OFF;}
};

class WaveGen {
public:
    typedef sFractional<0,31> out_sample_t;
    typedef float wave_sample_t;

	static inline void request_resetI(){
		pending_reset = true;
	}

	static inline bool is_running(){
		return state == ST_RUNNING;
	}

	typedef enum {
		ST_RESET    = 0,
		ST_READY    = 1,
		ST_RUNNING  = 2,
		ST_DONE     = 3,
		ST_STARTING = 4  //!< Told to start, but not necessarily ready
	} state_t;

	static constexpr uint32_t num_generators = 3;

	static inline state_t& get_state(){return state;}

	static inline bool is_ready() {
		return state == ST_READY;
	}

	/*!
	 @brief A callback to get samples
	 @param dst The destination buffer
	 @param n Ignored, number of samples requested(size known from \ref AK4621)

	 @note This should be called from interrupt or locked context

	 @note Generation will be performed immediately from this context due to
	 short deadline

	 @note Inlined because only a single caller
	 */
    static inline void get_samplesI(out_sample_t * dst, size_t n, uint32_t increment=2){
		// Zero the whole damn thing first
		zero16(dst, increment * (buffer_size / 2));

		// Check if muting
		if(silent){
			if(state == ST_RUNNING)
				state = ST_DONE;
			return;
		}

		auto t = get_time_ms();

		bool alldone = true;

		for(Generator &generator : generators){
			switch(generator.type){
			case Generator::GEN_OFF:
				// Don't care
				continue;
			case Generator::GEN_FIXED:
				// Go add a fixed wave to the appropriate channels
				generate_wave(generator, dst + generator.ch * (increment / 2), t, increment);
				break;
			case Generator::GEN_CHIRP:
				generate_chirp(generator, dst + generator.ch * (increment / 2), t, increment);
				break;
			// Add other signal generation handlers here if anything else req'd
			}

			alldone &= generator.t2 < t;
		}

		if(alldone){
			mute();
			Platform::led_blue.clear();
			state = ST_DONE;
		}
	}

	//! @name Tone selection functions
	//! Use to adjust tones being played. Please mute before using...
	//! @{
	static void set_chirp(uint8_t idx, uint8_t ch, uint16_t f1, uint16_t f2,
				   uint16_t t1, uint16_t t2, sFractional<7,8> gaindb){
		// state = ST_READY;
	}

	static void set_tone(uint8_t idx, uint8_t ch, uint16_t f1,
                  uint16_t t1, uint16_t t2, float gaindb);

	static inline void set_off(uint8_t idx){
		if(state == ST_RESET || state == ST_READY){
			generators[idx].type = Generator::GEN_OFF;
			state = ST_READY;
		}
	}
	//! @}

	static inline void runI(){
		if(state == ST_READY){
			state = ST_STARTING;
            Platform::codec.set_out_cb(get_samplesI);
			bool have_active = false;
			for(auto &gen : generators){
				if(gen.type != Generator::GEN_OFF)
					have_active = true;
			}
			if(have_active)
				unmute();
		}
	}

	static PT_THREAD(pt_wavegen(struct pt * pt)){
		PT_BEGIN(pt);

        Platform::codec.set_out_cb(get_samplesI);

		do_reset();

		while(true){
			// Check for reset request
			if(pending_reset){
				do_reset();
			}
			// Enable output if relevant
			if(state == ST_STARTING){
				state = ST_RUNNING;
				Platform::led_proc_wavegen.set();
				//if(!silent && !TPA6130A2::is_ready()){
				//	TPA6130A2::enable();
				//}
			}
			if((state != ST_RUNNING) && TPA6130A2::is_ready()){
				//TPA6130A2::disable();
			}

			PT_YIELD(pt);
		}

		PT_END(pt);
	}
protected:
	static constexpr wave_sample_t get_index(uint32_t index){
		return (index < wavetable_len) ? wavetable[index] :
		       ((index < wavetable_len * 2) ? wavetable[(wavetable_len - 1) - (index & (wavetable_len - 1))] :
		       0 - get_index(index - (wavetable_len * 2)));
	}
	
	static constexpr uint32_t buffer_size = AK4621::out_buffer_size;
	static constexpr uint32_t wavetable_len = 8192;
    static const wave_sample_t wavetable[wavetable_len];
	static constexpr uint32_t fs = AK4621::fs;

	static bool silent;

	static Generator generators[num_generators];

	static state_t state;

	//! @name Muting functions
	//! Mute all channels when altering parameters
	//! @{
	static inline void mute(){
		silent = true;
	}
	static inline void unmute(){
		silent = false;
	}
	//! @}

	/*!
	 @brief Generate a waveform
	 @param generator The generator to use
	 @param dst The start destination
	 @param t The start time of the frame to fill

	 This will generate \ref buffer_size / increment samples and add them to @ref dst
	 in alternating slots (to apply to a single channel only)
	 */
    static void generate_wave(Generator &generator, out_sample_t * dst, uint16_t t, uint32_t increment=2){
		if(t < generator.t1 || t > generator.t2){
			return;
		}

		// Just some temporary storage
		//*dst = *dst + (s * get_speaker_gain(generator.f1));
        wave_sample_t s;
		uint32_t theta = generator.theta;
        wave_sample_t gain = generator.gain;
		for(uint32_t i = 0; i < buffer_size / 2; i++){
			s = get_index(round_unsigned<uint32_t>(theta));

			s = s * gain;//__SSAT((((q63_t) s * gain) >> 32),31);
			//s = s * get_speaker_gain(generator.f1);

			// Add this sample to the output
            *dst = *dst + out_sample_t::fromInternal(clip_q63_to_q31(s * 2147483647.0f));

			dst += increment;

			theta = theta + generator.w1;
			if(theta >= wavetable_len * 4)
				theta -= wavetable_len * 4;
		}
		generator.theta = theta;
	}

	/*!
	 @brief Generate a chirp
	 @param generator The generator to use
	 @param dst The start destination
	 @param t The start time of the frame to fill

	 This will generate \ref buffer_size / 2 samples and add them to @ref dst
	 in alternating slots (to apply to a single channel only)
	 */
    static void generate_chirp(Generator &generator, out_sample_t * dst, uint16_t t, uint32_t increment=2){

	}

	static uint16_t get_time_ms();

	static bool pending_reset;

	static void do_reset(){
		Platform::led_proc_wavegen.clear();
		mute();
		set_off(0);
		set_off(1);
		set_off(2);
		state = ST_RESET;
		pending_reset = false;
	}

	static constexpr uint32_t speaker_coeffs_size = 256;
	// Starts at 100Hz
	static constexpr uint32_t speaker_coeffs_start = 100;
	// Divide frequency by 32 first
	static constexpr uint32_t speaker_coeffs_scaling = 32;

    static float speaker_coeffs[speaker_coeffs_size];

	/*!
	 @brief Compute the frequency-independent gain to be applied to a full-range
	 sine wave to achieve the desired gain.
	 */
    static float db_to_pp(float db){
        return std::pow(10.0f, (db - 119.5)/20.0);
		//return pow10f(db.asFloat()/20.0) / 4096;
	}

    static float get_speaker_gain(uint16_t freq){
        return speaker_coeffs[(freq - speaker_coeffs_start + (speaker_coeffs_scaling / 2)) / speaker_coeffs_scaling];
	}


};

#endif // __APULSE_WAVEGEN_H_


