#include <stm32h7xx_hal.h>

#include "ap.h"
#include "test_ctrl.h"
#include "audio_ctrl.h"


result_t test_reset(ap_test_t * test)
{
    result_t result = RET_UNKNOWN;

    EXIT_IF(test == NULL, RET_INVAL);

    CLR_PTR(test);
    test->state = AP_TEST_ST_IDLE;

    SUCCEED();

END:
    return result;
}

result_t test_start(ap_test_t * test, ap_test_config_t const * config)
{
    result_t result = RET_UNKNOWN;
    result_t int_result = RET_UNKNOWN;

    EXIT_IF(test == NULL || config == NULL, RET_INVAL);

    EXIT_IF(test->state != AP_TEST_ST_IDLE, RET_INVAL);

    int_result = audio_start_in(NULL, 0, NULL);


    SUCCEED();
END:
    return result;
}
