#include <stm32h7xx_hal.h>

#include "audio_ctrl.h"

#define AUDIO_CHANNELS_OUT (2)
#define AUDIO_CHANNELS_IN (1)

typedef struct {
    SAI_HandleTypeDef * sai_rx;
    ap_sample_t * rx_dest;
    uint32_t rx_size;
    audio_in_cb_t rx_cb;

    SAI_HandleTypeDef * sai_tx;
    ap_sample_t * tx_src;
    uint32_t tx_size;
    audio_out_cb_t tx_cb;
} audio_state_t;

static audio_state_t audio_state = {0};

void audio_deinit( )
{
    CLR_PTR(&audio_state);
}

result_t audio_init( SAI_HandleTypeDef * rx, SAI_HandleTypeDef * tx )
{
    result_t result = RET_UNKNOWN;

    EXIT_IF( rx == NULL || tx == NULL, RET_INVAL );
    EXIT_IF(audio_state.sai_rx != NULL || audio_state.sai_tx != NULL, RET_ERROR);

    audio_state.sai_rx = rx;
    audio_state.sai_tx = tx;

    SUCCEED();
END:
    return result;
}

result_t audio_reset()
{
    result_t result = RET_UNKNOWN;

    EXIT_IF( audio_state.sai_rx == NULL || audio_state.sai_tx == NULL, RET_INVAL );

    HAL_SAI_DMAStop(audio_state.sai_rx);
    HAL_SAI_DMAStop(audio_state.sai_tx);

    SUCCEED();
END:
    return result;
}

result_t audio_start_in( ap_sample_t * dest, uint32_t size, audio_in_cb_t cb )
{
    result_t result = RET_UNKNOWN;

    EXIT_IF( audio_state.sai_rx == NULL, RET_ERROR );
    EXIT_IF( dest == NULL || size == 0 || cb == NULL, RET_INVAL );

    audio_state.rx_cb = cb;
    audio_state.rx_dest = dest;
    audio_state.rx_size = size;

    // TODO: is size in B or words?
    HAL_SAI_Receive_DMA( audio_state.sai_rx, (uint8_t *)dest, size );

    SUCCEED();
END:
    return result;
}

result_t audio_start_out( ap_sample_t * src, uint32_t size, audio_out_cb_t cb )
{
    result_t result = RET_UNKNOWN;

    EXIT_IF( audio_state.sai_tx == NULL, RET_ERROR );
    EXIT_IF( src == NULL || size == 0 || cb == NULL, RET_INVAL );

    audio_state.tx_cb = cb;
    audio_state.tx_src = src;
    audio_state.tx_size = size;

    // TODO: is size in B or words?
    HAL_SAI_Transmit_DMA( audio_state.sai_tx, (uint8_t *)src, size );

    SUCCEED();
END:
    return result;
}

void HAL_SAI_TxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    if((hsai == audio_state.sai_tx) && (audio_state.tx_cb != NULL))
    {
        audio_state.tx_cb(audio_state.tx_src,
                          audio_state.tx_size / (2 * AUDIO_CHANNELS_OUT),
                          AUDIO_CHANNELS_OUT);
    }
}

void HAL_SAI_TxCpltCallback(SAI_HandleTypeDef *hsai)
{
    if((hsai == audio_state.sai_tx) && (audio_state.tx_cb != NULL))
    {
        audio_state.tx_cb(audio_state.tx_src + (audio_state.tx_size / 2),
                          audio_state.tx_size / (2 * AUDIO_CHANNELS_OUT),
                          AUDIO_CHANNELS_OUT);
    }
}

void HAL_SAI_RxHalfCpltCallback(SAI_HandleTypeDef *hsai)
{
    if((hsai == audio_state.sai_rx) && (audio_state.rx_cb != NULL))
    {
        audio_state.rx_cb(audio_state.rx_dest,
                          audio_state.rx_size / (2 * AUDIO_CHANNELS_IN),
                          AUDIO_CHANNELS_IN);
    }
}

void HAL_SAI_RxCpltCallback(SAI_HandleTypeDef *hsai)
{
    if((hsai == audio_state.sai_rx) && (audio_state.rx_cb != NULL))
    {
        audio_state.rx_cb(audio_state.rx_dest + (audio_state.rx_size / 2),
                          audio_state.rx_size / (2 * AUDIO_CHANNELS_IN),
                          AUDIO_CHANNELS_IN);
    }
}

void HAL_SAI_ErrorCallback(SAI_HandleTypeDef *hsai)
{
    UNUSED(hsai);
}
