
#ifndef __APULSE_CTRL_IF_H_
#define __APULSE_CTRL_IF_H_

#include <hal.h>

class CtrlIF {
public:
    static void Initialize();

    static void cmd_debug_text(char const * s);

private:
    enum cmd_opcode : uint16_t {
        OP_SET_LED = 1,
        OP_SET_POWER = 2,
        OP_DBG_TEXT = 3,
        OP_RET = 4,
        OP_TEST_BEGIN = 5,
        OP_TEST_ABORT = 6,
        OP_GET_STATUS = 7,
        OP_PUT_STATUS = 8,


        OP_SET_PARAM_F1 = 32,
        OP_SET_PARAM_F2 = 33,
        OP_SET_PARAM_A1 = 34,
        OP_SET_PARAM_A2 = 35,
        OP_SET_PARAM_TPLAY0 = 36,
        OP_SET_PARAM_TPLAYN = 37,
        OP_SET_PARAM_TACQ0 = 38,
        OP_SET_PARAM_TACQN = 39,
        OP_SET_PARAM_SOURCE = 40,
        OP_SET_PARAM_TARGET_BINS = 41,
        OP_SET_TEST_TYPE = 42,

        OP_GET_PSD = 64,
        OP_GET_SAMPLES = 65,
        OP_PUT_SAMPLES = 66,

    };

    struct cmd_header {
        uint16_t len;
        enum cmd_opcode op;
    } __attribute__((packed));

    enum status_capture : uint8_t {
        STAT_CAP_IDLE = 1,
        STAT_CAP_CAPTURE = 2,
        STAT_CAP_COMPLETE = 3,
        STAT_CAP_ERROR = 4
    };

    enum status_wavegen : uint8_t {
        STAT_WAV_IDLE = 1,
        STAT_WAV_WAIT = 2,
        STAT_WAV_GEN = 3,
        STAT_WAV_COMPLETE = 4,
        STAT_WAV_ERROR = 5
    };

    enum status_analysis : uint8_t {
        STAT_ANA_IDLE = 1,
        STAT_ANA_WAIT = 2,
        STAT_ANA_PROCESSING = 3,
        STAT_ANA_RESULT_READY = 4,
        STAT_ANA_ERROR = 5
    };

    enum status_test : uint8_t {
        STAT_TEST_IDLE = 1,
        STAT_TEST_RUNNING = 2,
        STAT_TEST_COMPLETE = 3,
        STAT_TEST_ERROR = 4
    };

    struct cmd_status {
        enum status_analysis stat_analysis;
        enum status_capture stat_capture;
        enum status_wavegen stat_wavegen;
        enum status_test stat_test;
    } __attribute__((packed));
    
    static constexpr uint32_t thread_stack_size = 8192;
    static constexpr uint32_t usb_write_timeout = 500;
    static constexpr uint32_t usb_read_timeout = 500;
    
    inline static THD_WORKING_AREA(thread_wa, thread_stack_size);
    static THD_FUNCTION(thread_fn, arg);
    

};

#endif // __APULSE_CTRL_IF_H_
