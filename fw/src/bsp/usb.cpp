#include <bsp/usb.h>
#include <bsp/platform.h>

using namespace K20;

extern "C" {
	//#include "usb_audio.h"
	#include "usb_hid.h"
	#include "audio_generator.h"
	extern const unsigned char wav_data[];
	extern const uint_16 wav_size;
	extern uint_8 g_cur_mute[USB_MAX_SUPPORTED_INTERFACES];
};

uint_8 audio_sample;
uint_8 audio_event;
uint_8 audio_data_recv[8];


/*****************************************************************************
 * Local Variables
 *****************************************************************************/
#ifdef _MC9S08JS16_H
	#pragma DATA_SEG APP_DATA
#endif
/* Audio speaker Application start Init Flag */
static volatile boolean start_app = FALSE;
static boolean start_send = TRUE;
static int16_t wav_buff16[8];
static uint_32 audio_position = 0;

void USB::hidClassInit(){
	__disable_irq();

	uint8_t error = USB_Class_HID_Init(CONTROLLER_ID, callback, nullptr, callback_param);

	if(error != USB_OK)
		while(true);

	__enable_irq();
}

void USB::audioClassInit(){
#if 0
	uint8_t error;
	
	__disable_irq();

	// Demo used SCI. I don't
	sci_init();
     
    // Initialize the USB interface
	error = USB_Class_Audio_Init(CONTROLLER_ID, callback,
                                 NULL,NULL);
    if(error != USB_OK)
		while(true);
		
	__enable_irq();
#endif
}

void USB::audioSend(){
	// Check whether enumeration is complete or not
	if((start_app == TRUE) && (start_send == TRUE))
	{
		start_send = FALSE;
		prepare_data();
		//(void)USB_Class_Audio_Send_Data(CONTROLLER_ID, AUDIO_ENDPOINT, (uint8_t*)wav_buff16, sizeof(wav_buff16)) ;
	}
}

void USB::prepare_data(){
    uint_8 k;
    //if (g_cur_mute[0] == 0)
	if (true)
    {
		/* copy data to buffer */
		for(k=0;k<8;k++,audio_position++)
		{
			wav_buff16[k] = (wav_data[audio_position] - 127) << 8;
			//wav_buff16[k] = 0x7FF0 * (audio_position & 1);
		}
    }
    else
    {
		/* copy data to buffer */
		for(k=0;k<8;k++,audio_position++)
		{
			wav_buff16[k] = 0;
			//wav_buff16[k] = wav_data[audio_position];
			// wav_buff16[k] = 0x7FF0 * (audio_position & 1);
		}
    }
}

void USB::callback(uint8_t controller_ID, uint8_t event_type, void *val){
	if(event_type == USB_APP_DATA_RECEIVED){
		__BKPT(10);
	} else if(event_type == USB_APP_SEND_COMPLETE){
		__BKPT(5);
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
