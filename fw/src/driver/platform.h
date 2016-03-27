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

#ifndef __APULSE_PLATFORM_H_
#define __APULSE_PLATFORM_H_

#ifdef __cplusplus

#include <driver/clocks.h>
#include <driver/usb.h>
#include <driver/gpio.h>
#include <driver/codec.h>
#include <driver/tpa6130a2.h>
#include <driver/pwm_gpio.h>
#include <driver/spi.h>
#include <driver/mt29fxg01.h>
#include <driver/uart.h>
#include <pt.h>

class Platform {
public:
	/*!
	 * @brief Initialization to be done before anything
	 *
	 * @note Since this may be done before data initialization, it may only use
	 * constants
	 */
	static void earlyInit();

	/*!
	 @brief Initialize peripherals and devices that depend on clocks and data initialization
	 */
	static void lateInit(){
		USB::lateInit();

        Platform::codec.init_hw();

		TPA6130A2::init_hw();

		leds[0].clear();
		leds[1].clear();
		leds[2].clear();
	}

	static PWMFTM const pwm_ftm;

	static GPIOPin const leds[3];
	static PWMGPIOPin const pwm[3];

	static SPI spi0;
	static SPI_slave codec_slave;

    static AK4621 codec;

    static MT29FxG01 flash;
    static SPI_slave flash_slave;

	static void power_on(){power_en.set();}
#if CFG_POWER_ALWAYS_ON
    static void power_off(){}
#else
	static void power_off(){power_en.clear();}
#endif

	//! The external 12.288MHz oscillator
	static GPIOPin const xtal_ex;
    static GPIOPin const uart_tx;
    static GPIOPin const uart_rx;

    typedef bool (*tick_cb_t)(void *);
    typedef struct _task_t {
        uint32_t period;
        tick_cb_t cb;
        void * ptr;
        uint32_t remaining;
        struct _task_t * next;
    } task_t;

    static void add_task(uint32_t period, tick_cb_t cb, void * ptr, task_t * buffer){


        buffer->period = period;
        buffer->cb = cb;
        buffer->ptr = ptr;
        buffer->remaining = period;
        buffer->next = nullptr;

        __disable_irq();

        if(head){
            task_t * iter = head;
            while(iter->next)
                iter = iter->next;
            iter->next = buffer;
        } else {
            head = buffer;
        }

        __enable_irq();
    }

    static task_t * head;

    static void tick(){
        task_t * iter;
        task_t * last_iter = nullptr;
        iter = head;
        while(iter){
            iter->remaining -= 1;
            if(iter->remaining == 0){
                if(iter->cb(iter->ptr)){
                    // Continue
                    iter->remaining = iter->period;
                } else {
                    // Die
                    if(last_iter){
                        last_iter->next = iter->next;
                        iter = iter->next;
                        continue;
                    }
                }
            }

            last_iter = iter;
            iter = iter->next;
        }
    }

private:
	//! Analog power enable
	static GPIOPin const power_en;


};

#endif // __cplusplus


#ifdef __cplusplus
extern "C" {
#endif
/*!
 * @brief An earlyInit with C-ready linkage
 */
void earlyInitC();

// SPI DMA ISRs
void DMA_CH2_ISR();
void DMA_CH3_ISR();

void SPI0_ISR();

// Codec DMA ISRs
void DMA_CH0_ISR();
void DMA_CH1_ISR();

void SysTick_ISR();

#ifdef __cplusplus
};
#endif

#endif // __APULSE_PLATFORM_H_

