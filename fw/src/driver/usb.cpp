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

#include <driver/usb.h>
#include <driver/platform.h>
#include <controller.h>
#include <usb_descriptor.h>


extern "C" {
	#include "usb_hid.h"
	#include "usb_cdc.h"
};


static uint8_t test[64];


CLASS_APP_CALLBACK_STRUCT USB::cdc_class_callback_struct = {
	USB::CDCAppCallback,
	nullptr,
	USB::CDCNotifyCallback,
	nullptr,
};

CLASS_APP_CALLBACK_STRUCT USB::hid_class_callback_struct = {

	HIDcallback,
	nullptr,
	HIDcallback,
	HIDParamCallback
};

CLASS_APP_CALLBACK_STRUCT USB::audio_class_callback_struct = {
	AudioCallback,
	nullptr,
	nullptr, //AudioCallback, //AudioParamCallback,
	nullptr
};

COMPOSITE_CALLBACK_STRUCT USB::usb_composite_callback ={
    COMP_CLASS_UNIT_COUNT,
    {
        &audio_class_callback_struct,
		//&hid_class_callback_struct,
		&cdc_class_callback_struct,
	}
};

bool USB::audio_ready = false;
bool USB::audio_empty = true;
bool USB::hid_ready   = false;
uint32_t USB::audio_packets_in_flight = 0;
USB::AudioQueue USB::queue;

static uint32_t log_hid_callback_count = 0;
static volatile uint8_t log_hid_callback_events[64] = {};
void USB::HIDcallback(uint8_t controller_ID,
	              uint8_t event_type, void * val){
    if(log_hid_callback_count < 64){
        log_hid_callback_events[log_hid_callback_count++] = event_type;
    }
    APulseController::handle_eventI(event_type);

	if((event_type == USB_APP_BUS_RESET) || (event_type == USB_APP_CONFIG_CHANGED)){
		hid_ready = false;
//		keyboard_init=FALSE;
	} else if(event_type == USB_APP_ENUM_COMPLETE) {
		// if enumeration is complete set keyboard_init so that
		// application can start 
//		keyboard_init=TRUE;
		hid_ready = true;
	} else if(event_type == USB_APP_ERROR) {
        __BKPT();
		// user may add code here for error handling
		while(true);
    } else if(event_type == USB_APP_SEND_COMPLETE) {
        // Well, not much to do here now...
	} else if (event_type == USB_APP_BUS_SUSPEND) {
		// This is the app callback, so we shouldn't halt the ISR by doing
		// anything here.
//		main_App_Should_Suspend = keyboard_init;
	} else if (event_type == USB_APP_BUS_RESUME) {
		/* For the keyboard application, it is best to let the main app 
		 * handle all resume processing. Because of the software flow, we know
		 * that after initiating the STOP mode and waking up, the application
		 * will just continue, so we can resume there.
		 */
    } else {
        static volatile uint8_t e = event_type;
        __BKPT();
    }
}

static uint32_t log_audio_callback_count = 0;
static volatile uint8_t log_audio_callback_events[64] = {};

void USB::AudioCallback(uint8_t controller_ID,
			  uint8_t event_type, void * val){
	if(log_audio_callback_count < 64){
		log_audio_callback_events[log_audio_callback_count++] = event_type;
	}
	uint8_t * ptr = nullptr;
	uint16_t len;
	
	switch(event_type){
	case USB_APP_BUS_RESET:      // 0
		audio_ready = false;
		break;
	case USB_APP_CONFIG_CHANGED: // 1
		break;
	case USB_APP_ENUM_COMPLETE:  // 2
		audio_ready = true;
		break;
	case USB_APP_SEND_COMPLETE:  // 3
        audio_packets_in_flight -= 1;
        audio_empty = (audio_packets_in_flight == 0);
		queue.pop();
		len = queue.peek(&ptr);
		if(len){
			audioSendData(ptr, len);
		}
		break;
	case USB_APP_DATA_RECEIVED:  // 4
		while(true);
		break;
	case USB_APP_ERROR:          // 5
		while(true);
	case USB_APP_BUS_SUSPEND:    // 10
        audio_ready = false;
		break;
	case USB_APP_BUS_RESUME:     // 12
        audio_ready = true;
		break;
	default:
		while(true);
    }
    
    return;
}

void USB::audioSendData(uint8_t const * buffer, uint16_t n){
	//USB_Class_Send_Data(CONTROLLER_ID, AUDIO_ENDPOINT, buffer, n);
	if(audio_ready){
		USB_Class_Audio_Send_Data(CONTROLLER_ID, AUDIO_ENDPOINT, (uint8_t *)buffer, n);
        audio_packets_in_flight += 1;
        audio_empty = false;
	}
}

bool USB::audioSend(float const * buffer, uint16_t n){
	uint8_t * ptr = nullptr;
	uint16_t len;
    auto result = queue.push(buffer, n);
    if(audio_packets_in_flight < 1){
		len = queue.peek(&ptr);
		if(len)
			audioSendData(ptr, len);
	}
    return result;
}



uint8_t USB::AudioParamCallback(uint_8 request, uint_16 value, uint_16 wIndex,
                                uint8_t** data, uint_16* size)
{
	while(1);
}


uint8_t USB::HIDParamCallback(uint_8 request, uint_16 value,
                              uint_16 wIndex, uint8_t** data, uint_16* size){
	// Request buffers
	//static uint8_t report_buf[64] = {'a','b','a','a','c'};
	static uint8_t protocol_req = 0;
	static uint8_t idle_req = 0;
	
	uint8_t status = USB_OK;
	
	uint8_t index = (uint8_t)((request - 2) & USB_HID_REQUEST_TYPE_MASK);
                                         // index == 0 for get/set idle,
                                         // index == 1 for get/set protocol
	
	*size = 0;
	
	switch(request){
	case USB_HID_GET_REPORT_REQUEST :
        __BKPT();
		*data = APulseController::get_response(*size);
		break;
	case USB_HID_SET_REPORT_REQUEST :
        __BKPT();
		APulseController::handle_dataI(*data, *size);
// 		for(uint32_t i = 0; i < 64; i++){
// 			report_buf[i] = (*data)[i];
// 		}
		
		if(**data & 0x01) {	// turn NumLock led on request from HOST
			
		} else { // turn NumLock led off request from HOST
			
		}
		
		if(**data & 0x02) {	// turn CapsLock led on request from HOST
			
		}
		break;

	case USB_HID_GET_IDLE_REQUEST :
        __BKPT();
		// Point to the current idle rate
		*data = &idle_req;
		*size = 1;
//		*data = &g_app_request_params[index];
//		*size = REQ_DATA_SIZE;
		break;

	case USB_HID_SET_IDLE_REQUEST :
        __BKPT();
		// set the idle rate sent by the host
		if(index < 2){
			// This came from control EP?
			idle_req = value >> 8;
		//	g_app_request_params[index] =(uint_8)((value & MSB_MASK) >>
		//                                          HIGH_BYTE_SHIFT);
		}
		break;

	case USB_HID_GET_PROTOCOL_REQUEST :
        __BKPT();
		/* point to the current protocol code
		0 = Boot Protocol
		1 = Report Protocol*/
		*data = &protocol_req;
		*size = 1;
//		*data = &g_app_request_params[index];
//		*size = REQ_DATA_SIZE;
		break;

	case USB_HID_SET_PROTOCOL_REQUEST :
        __BKPT();
		/* set the protocol sent by the host
		0 = Boot Protocol
		1 = Report Protocol*/
		if(index < 2) {
			protocol_req = value >> 8;
			//g_app_request_params[index] = (uint_8)(value);
		}
		break;
    default:
        __BKPT();
	}

	return status;
}


void USB::hidClassInit(){
	//__disable_irq();
	
	//uint8_t error = USB_Class_HID_Init(CONTROLLER_ID, callback, nullptr, callback_param);
	uint8_t error = USB_Composite_Init(CONTROLLER_ID, &usb_composite_callback);
	
	if(error != USB_OK)
		while(true);

	//__enable_irq();
	
	for(auto &a : test) a = 'p';
}


void USB::CDCNotifyCallback(uint_8 controller_ID, uint_8 event_type, void * val){

}



void USB::CDCAppCallback(uint_8 controller_ID,   /* [IN] Controller ID */
						 uint_8 event_type,      /* [IN] value of the event */
						 void * val){

}
