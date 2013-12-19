#include <bsp/usb.h>
#include <bsp/platform.h>

using namespace K20;

extern "C" {
	//#include "usb_audio.h"
	#include "usb_hid.h"
	#include "audio_generator.h"
};


static uint8_t test[64];


void USB::hidClassInit(){
	__disable_irq();

	uint8_t error = USB_Class_HID_Init(CONTROLLER_ID, callback, nullptr, callback_param);

	if(error != USB_OK)
		while(true);

	__enable_irq();
	
	for(auto &a : test) a = 'p';
	USB_Class_HID_Send_Data(CONTROLLER_ID, 0, test, 64);
}

void USB::callback(uint8_t controller_ID, uint8_t event_type, void *val){
	if(event_type == USB_APP_DATA_RECEIVED){
		
	} else if(event_type == USB_APP_SEND_COMPLETE){
		
	}
    return;
}

uint8_t USB::callback_param(uint8_t request, uint16_t value, uint16_t iface,
                            uint8_t **data, uint_16 *size){

	uint8_t direction =  (uint8_t)((request & USB_HID_REQUEST_DIR_MASK) >>3);
	uint8_t index = (uint8_t)((request - 2) & USB_HID_REQUEST_TYPE_MASK);
                                         // index == 0 for get/set idle,
                                         // index == 1 for get/set protocol

	switch(request){
	case USB_HID_GET_REPORT_REQUEST:
		Platform::led.clear();
		break;
	case USB_HID_SET_REPORT_REQUEST:
		Platform::led.set();
		break;
	case USB_HID_SET_IDLE_REQUEST:
		break;
	default:
		__BKPT(1);
		break;
	}

	return USB_OK;
}
