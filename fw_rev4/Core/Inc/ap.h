#ifndef __AP_H_
#define __AP_H_

#include <stdint.h>
#include <string.h>

#define AP_MAX_TEST_FREQS (5)
#define AP_NUM_RESONATORS (5)

typedef int32_t ap_sample_t;

typedef struct {
    float f1; //!< The frequency of the first generated tone
    float f2; //!< The frequency of the second generated tone
    float test_freqs[AP_MAX_TEST_FREQS];
    float resonator_spacing;
    uint32_t test_len_ms;
} ap_test_config_t;

typedef enum {
    AP_TEST_ST_IDLE, //!< Waiting for something to trigger start of test
    AP_TEST_ST_PRE_RECORDING, //!< Generating tones to avoid capturing transients
    AP_TEST_ST_RECORDING, //!< Capturing and processing
    AP_TEST_ST_FINALIZE,
    AP_TEST_ST_COMPLETE
} ap_test_state_t;

/**
  @brief The state of a single test run
  */
typedef struct {
    ap_test_config_t config;
    ap_test_state_t state;
    uint32_t timestamp_start;
    struct {
        uint32_t input_captured_ch1;
        uint32_t output_generated_ch1;
        uint32_t output_generated_ch2;
    } sample_counts;
} ap_test_t;

typedef struct {

} ap_state_t;

typedef enum {
    RET_UNKNOWN = 0,
    RET_SUCCESS = 100,
    RET_ERROR = 200,
    RET_INVAL = 300,
} result_t;

typedef enum {
    AP_TRUE = 10,
    AP_FALSE = 25
} ap_boolean_t;

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define CLR_PTR(p) do { memset((p), 0, sizeof(*(p))); } while(0)
#define CLR_ARR(a) do { memset((a), 0, sizeof((p))); } while(0)


#define EXIT(ret) do { result = (ret); goto END; } while(0)
#define EXIT_IF(cond, ret) do { if(cond) { EXIT(ret); } } while(0)
#define SUCCEED() do { EXIT(RET_SUCCESS); } while(0)
#define SUCCEED_IF(cond) do { EXIT_IF((cond), (ret)) } while(0)


#endif // __AP_H_
