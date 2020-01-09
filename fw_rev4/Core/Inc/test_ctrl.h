#ifndef __TEST_CTRL_H_
#define __TEST_CTRL_H_

#include "ap.h"

result_t test_reset(ap_test_t * test);
result_t test_start(ap_test_t * test, ap_test_config_t const * config);

#endif // __TEST_CTRL_H_
