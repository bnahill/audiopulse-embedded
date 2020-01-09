#ifndef __AUDIO_CTRL_H_
#define __AUDIO_CTRL_H_

#include <stdint.h>
#include <stm32h7xx_hal.h>

#include "ap.h"

typedef void (*audio_out_cb_t)(ap_sample_t * samples, uint32_t frames, uint32_t nchannels);
typedef void (*audio_in_cb_t)(ap_sample_t const * samples, uint32_t frames, uint32_t nchannels);

void audio_deinit( );
result_t audio_init( SAI_HandleTypeDef * rx, SAI_HandleTypeDef * tx );
result_t audio_reset( );

/**
 * @brief Start audio input to dest. Callbacks will come when half of the data is available.
 * @param dest
 * @param size
 * @return
 */
result_t audio_start_in( ap_sample_t * dest, uint32_t size, audio_in_cb_t cb );
result_t audio_start_out( ap_sample_t * src, uint32_t size, audio_out_cb_t cb );

#endif // __AUDIO_CTRL_H_
