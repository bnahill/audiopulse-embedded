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


extern "C" {
	#include "usb_hid.h"
};


static uint8_t test[64];


void USB::hidClassInit(){
	__disable_irq();

	uint8_t error = USB_Class_HID_Init(CONTROLLER_ID, callback, nullptr, callback_param);

	if(error != USB_OK)
		while(true);

	__enable_irq();
	
	for(auto &a : test) a = 'p';
}

void USB::callback(uint8_t controller_ID, uint8_t event_type, void *val){
	if((event_type == USB_APP_BUS_RESET) || (event_type == USB_APP_CONFIG_CHANGED)){
//		keyboard_init=FALSE;
	} else if(event_type == USB_APP_ENUM_COMPLETE) {
		// if enumeration is complete set keyboard_init so that
		// application can start 
//		keyboard_init=TRUE;
	} else if(event_type == USB_APP_ERROR) {
		// user may add code here for error handling
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
	}
}

uint8_t USB::callback_param(uint8_t request, uint16_t value, uint16_t iface,
                            uint8_t **data, uint_16 *size){
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
		*data = APulseController::get_response(*size);
		break;
	case USB_HID_SET_REPORT_REQUEST :
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
		// Point to the current idle rate
		*data = &idle_req;
		*size = 1;
//		*data = &g_app_request_params[index];
//		*size = REQ_DATA_SIZE;
		break;

	case USB_HID_SET_IDLE_REQUEST :
		// set the idle rate sent by the host
		if(index < 2){
			// This came from control EP?
			idle_req = value >> 8;
		//	g_app_request_params[index] =(uint_8)((value & MSB_MASK) >>
		//                                          HIGH_BYTE_SHIFT);
		}
		break;

	case USB_HID_GET_PROTOCOL_REQUEST :
		/* point to the current protocol code
		0 = Boot Protocol
		1 = Report Protocol*/
		*data = &protocol_req;
		*size = 1;
//		*data = &g_app_request_params[index];
//		*size = REQ_DATA_SIZE;
		break;

	case USB_HID_SET_PROTOCOL_REQUEST :
		/* set the protocol sent by the host
		0 = Boot Protocol
		1 = Report Protocol*/
		if(index < 2) {
			protocol_req = value >> 8;
			//g_app_request_params[index] = (uint_8)(value);
		}
		break;
	}

	return status;
}
