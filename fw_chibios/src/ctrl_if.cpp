/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "hal.h"
#include "hal_streams.h"
#include "chprintf.h"
#include "board.h"
#include "string.h"

#include "ctrl_if.h"
#include "apulse_util.h"
#include "codec.h"
#include "platform.h"

typedef struct {
    uint8_t led;
    uint8_t value;
} __attribute__((packed)) payload_set_led_t;

typedef struct {
    uint8_t value;
} __attribute__((packed)) payload_set_power_t;


//THD_WORKING_AREA(CtrlIF::thread_wa, thread_stack_size);

THD_FUNCTION(CtrlIF::thread_fn, arg){
    (void)arg;
    chRegSetThreadName("CtrlIF");
    static uint8_t cmd_buffer[1024] = {0};
    uint16_t cmd_len;
    uint16_t cmd_op;
    uint16_t nbytes;

    USBSerial::init();
    Platform::pin_pwr.set();
    chThdSleepMilliseconds(50);
    Platform::codec.init_hw();
    Platform::codec.init();
    Platform::codec.start();

    while (true) {
        chThdSleepMilliseconds(10);

        nbytes = USBSerial::read_timeout(reinterpret_cast<uint8_t *>(&cmd_len), 2, usb_read_timeout);
        if((nbytes != 2) || (cmd_len > sizeof(cmd_buffer)))
            continue;
        nbytes = USBSerial::read_timeout(reinterpret_cast<uint8_t *>(&cmd_op), 2, usb_read_timeout);
        if(nbytes != 2)
            continue;
        if(cmd_len){
            nbytes = USBSerial::read_timeout(cmd_buffer, cmd_len, usb_read_timeout);
            if(nbytes != cmd_len)
                continue;
        }
        
        switch(cmd_op){
        case OP_SET_LED:
            {
                if(cmd_len != sizeof(payload_set_led_t)) continue;
                payload_set_led_t * payload = reinterpret_cast<payload_set_led_t *>(cmd_buffer);
                switch (payload->led) {
                    case 1:
                        palWriteLine(LINE_LED1, (payload->value) ? PAL_HIGH : PAL_LOW);
                        break;
                    case 2:
                        palWriteLine(LINE_LED2, (payload->value) ? PAL_HIGH : PAL_LOW);
                        break;
                    case 3:
                        palWriteLine(LINE_LED3, (payload->value) ? PAL_HIGH : PAL_LOW);
                        break;
                    default:
                        break;
                }
            }
            break;
        case OP_SET_POWER:
            {
                if(cmd_len != sizeof(payload_set_power_t)) continue;
                payload_set_power_t * payload = reinterpret_cast<payload_set_power_t *>(cmd_buffer);
                Platform::pin_pwr.assign(payload->value);
            }
            break;
        case OP_GET_STATUS:
            {
                struct {
                    struct cmd_header header;
                    struct cmd_status payload;
                } __attribute__((packed)) resp;
                resp.header.op = OP_PUT_STATUS;
                resp.header.len = sizeof(resp.payload);

                resp.payload.stat_capture = STAT_CAP_IDLE;
                resp.payload.stat_analysis = STAT_ANA_IDLE;
                resp.payload.stat_wavegen = STAT_WAV_IDLE;
                resp.payload.stat_test = STAT_TEST_IDLE;
                USBSerial::write(reinterpret_cast<uint8_t *>(&resp), sizeof(resp));
            }
            break;
        case OP_SET_PARAM_A1:
            break;

        default:
            break;
        }
    }
}

void CtrlIF::cmd_debug_text(const char *s){
    struct cmd_header header = {
        narrow_cast<uint16_t, size_t>(strlen(s) + 1),
        OP_DBG_TEXT
    };

    USBSerial::write(reinterpret_cast<uint8_t const *>(&header), sizeof(header));
    USBSerial::write(reinterpret_cast<uint8_t const *>(s), header.len);
}

void CtrlIF::Initialize(){
    chThdCreateStatic(thread_wa, sizeof(thread_wa), LOWPRIO, thread_fn, NULL);
}
