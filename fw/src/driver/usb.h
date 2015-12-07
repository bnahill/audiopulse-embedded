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

#ifndef __APULSE_USB_H_
#define __APULSE_USB_H_

#include <derivative.h>
#include <usb_class.h>
#include <driver/clocks.h>
#include <usb_composite.h>
#include <usb_audio.h>
#include <arm_math.h>

class USB {
public:
	static void lateInit(){
		static_assert(Clock::config.mcgoutclk == 48000000 or
		              Clock::config.mcgoutclk == 72000000 or
		              Clock::config.mcgoutclk == 120000000, "Invalid busclk value");
		switch(Clock::config.mcgoutclk){
		case 48000000:
			// Divide 48MHz by 1 for USB 48MHz
			SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(0);
			break;
		case 72000000:
			// Multiply 72MHz by 2 and then divide by 3
			SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(2) | SIM_CLKDIV2_USBFRAC_MASK;
			break;
		case 120000000:
			// Multiply 120MHz by 2 and then divide by 5
			SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(4) | SIM_CLKDIV2_USBFRAC_MASK;
			break;
		default:
			break;
		}

		// Enable USB-OTG IP clocking
		SIM_SCGC4 |= SIM_SCGC4_USBOTG_MASK;
		
		// Configure USB to be clocked from PLL
		SIM_SOPT2 |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL(1);
		
		USB0_CLK_RECOVER_IRC_EN = 0;
		

		// old documentation writes setting this bit is mandatory
		USB0_USBTRC0 = 0x40;

		// Configure enable USB regulator for device
		// SIM_SOPT1 |= SIM_SOPT1_USBREGEN_MASK;

		// ICPR[2] & (1 << 9) for K20
		NVIC->ICPR[1] = (1 << 21);	// Clear any pending interrupts on USB
		NVIC->ISER[1] = (1 << 21);	// Enable interrupts from USB module
	}

	static void hidClassInit();
	static void audioClassInit();
	static void audioSend(float const * buffer, uint16_t n);
	static constexpr size_t audioMaxSamples = AUDIO_ENDPOINT_PACKET_SIZE / AUDIO_ENDPOINT_SAMPLE_SIZE;
	
	static bool audioQueueFull(){return queue.isFull();}
	static bool audioReady(){return audio_ready;}
	static bool audioEmpty(){return audio_empty;}
	static bool hidReady(){return hid_ready;}
	
private:
	static bool audio_ready;
	static bool hid_ready;
	static bool audio_empty;
	
	static void audioSendData(uint8_t const * buffer, uint16_t n);
	
	/*!
	 * A class to represent a queue of audio sample sets to be pushed over USB
	 * when required. Deals with sample conversion and provides buffering
	 * for samples.
	 */
	class AudioQueue {
	public:
		AudioQueue() :
			count(0), rd_ptr(0), wr_ptr(0)
		{}
		bool isFull(){
			return count == num_elem;
		}
		
		bool isEmpty(){
			return count == 0;
		}
		
		/*!
		 * Look at the first element to use it
		 */
		uint32_t peek(uint8_t ** ptr){
			*ptr = mem[rd_ptr].data;
			return mem[rd_ptr].len;
		}
		
		/*!
		 * Actually get rid of that first element
		 */
		bool pop(){
			__disable_irq();
			if(!isEmpty()){
				count -= 1;
				rd_ptr = (rd_ptr == num_elem - 1) ? 0 : (rd_ptr + 1);
				__enable_irq();
				return true;
			} else {
				__enable_irq();
				return false;
			}
		}
		
		bool push(int32_t const * data, uint32_t nsamples){
			queue_elem_t &store = mem[wr_ptr];
			int32_t * iter = (int32_t*)store.data;
			if(nsamples > audioMaxSamples)
				return false;
			if(isFull())
				return false;
			if(AUDIO_ENDPOINT_SAMPLE_SIZE == 4){
				arm_copy_q31((int32_t*)data, (int32_t*)store.data, nsamples);
			} else {
				for(auto i = 0; i < nsamples; i++){
					if(AUDIO_ENDPOINT_SAMPLE_SIZE == 3){
						*iter = (*data >> 8) & 0x00FFFFFF;
					} else {
						return false;
					}
					iter += 1;
					data += 1;
				}
			}
			wr_ptr = (wr_ptr == num_elem - 1) ? 0 : (wr_ptr + 1);
			count += 1;
			store.len = nsamples * AUDIO_ENDPOINT_SAMPLE_SIZE;
			return true;
		}
		
		bool push(float const * data, uint32_t nsamples){
			queue_elem_t &store = mem[wr_ptr];
			uint8_t * iter = store.data;
			int32_t sample;
			float fsample;
			if(nsamples > audioMaxSamples)
				return false;
			if(isFull())
				return false;
			for(auto i = 0; i < nsamples; i++){
				if(AUDIO_ENDPOINT_SAMPLE_SIZE == 4){
					fsample = (*data * 2147483648.0f);
					// Round it
					fsample += fsample > 0.0f ? 0.5f : -0.5f;
					*((uint32_t *)iter) = clip_q63_to_q31((q63_t) (fsample));
					iter += 4;
				} else if(AUDIO_ENDPOINT_SAMPLE_SIZE == 3){
					fsample = (*data * 2147483648.0f / 256.0f);
					// Round it
					fsample += fsample > 0.0f ? 0.5f : -0.5f;
					sample = clip_q63_to_q31((q63_t) (fsample));
					*((uint32_t *)iter) = sample & 0x00FFFFFF;
					iter += 3;
				} else {
					return false;
				}
				data += 1;
			}
			wr_ptr = (wr_ptr == num_elem - 1) ? 0 : (wr_ptr + 1);
			count += 1;
			store.len = nsamples * AUDIO_ENDPOINT_SAMPLE_SIZE;
			return true;
		}
		
		
	protected:
		static constexpr uint32_t num_elem = 5;
		static constexpr uint32_t sample_size = 3;
		typedef struct {
			uint16_t len;
			uint8_t data[AUDIO_ENDPOINT_PACKET_SIZE];
			uint32_t extra_leg_room;
		} queue_elem_t;
		queue_elem_t mem[num_elem];
		
		uint8_t count;
		uint8_t rd_ptr;
		uint8_t wr_ptr;
	};
	
	static AudioQueue queue;
	
	static void HIDcallback(uint8_t controller_ID,
	                        uint8_t event_type, void * val);
	static uint8_t HIDParamCallback(uint_8 request,
	                                uint_16 value,
	                                uint_16 wIndex,
	                                uint8_t ** data,
	                                USB_PACKET_SIZE * size);
	static void AudioCallback(uint8_t controller_ID,
	                          uint8_t event_type, void * val);
	static uint8_t AudioParamCallback(uint_8 request,
	                                  uint_16 value,
	                                  uint_16 wIndex,
	                                  uint8_t ** data,
	                                  USB_PACKET_SIZE * size);
	
	
	
	static CLASS_APP_CALLBACK_STRUCT hid_class_callback_struct;
	static CLASS_APP_CALLBACK_STRUCT audio_class_callback_struct;
	static COMPOSITE_CALLBACK_STRUCT usb_composite_callback;

	/*!
	 * \brief callback_param
	 * \param request [IN] request type
	 * \param value [IN] report type and ID
	 * \param data [OUT] pointer to the data
	 * \param size [OUT] size of the transfer
	 * \return
	 */
	static uint8_t callback_param(uint8_t request, uint16_t value,
								  uint16_t iface, uint8_t ** data,
								  USB_PACKET_SIZE * size);

	static void prepare_data();

	static constexpr uint8_t CONTROLLER_ID = 0;
};


#endif // __APULSE_USB_H_
