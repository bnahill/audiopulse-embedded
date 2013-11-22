#include <bsp/usb.h>

using namespace K20;

extern "C" {
	#include "usb_audio.h"
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
static uint_8 wav_buff[8];
static int16_t wav_buff16[8];
static uint_32 audio_position = 0;

void USB::audioClassInit(){
	uint8_t error;
	
	__disable_irq();

	// Demo used SCI. I don't
	// sci_init();
     
    // Initialize the USB interface
	error = USB_Class_Audio_Init(CONTROLLER_ID, callback,
                                 NULL,NULL);
    if(error != USB_OK)
		while(true);
		
	__enable_irq();
}

void USB::audioSend(){
	// Check whether enumeration is complete or not
	if((start_app == TRUE) && (start_send == TRUE))
	{
		start_send = FALSE;
		prepare_data();
		(void)USB_Class_Audio_Send_Data(CONTROLLER_ID, AUDIO_ENDPOINT, (uint8_t*)wav_buff16, sizeof(wav_buff16)) ;
	}
}

void USB::prepare_data(){
    uint_8 k;
    if (g_cur_mute[0] == 0)
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
    if ((event_type == USB_APP_BUS_RESET) || (event_type == USB_APP_CONFIG_CHANGED))
    {
        start_app=FALSE;
    }
    else if(event_type == USB_APP_ENUM_COMPLETE)
    {
        start_app=TRUE;
        if (start_send == FALSE)
        {
		  prepare_data();
          (void)USB_Class_Audio_Send_Data(CONTROLLER_ID, AUDIO_ENDPOINT, (uint8_t*)wav_buff16, sizeof(wav_buff16));
        }
    }
    else if((event_type == USB_APP_SEND_COMPLETE) && (TRUE == start_app))
    {	
        	if(audio_position > wav_size)
        	{
        		audio_position = 0;
        	}
			prepare_data();
        	(void)USB_Class_Audio_Send_Data(controller_ID, AUDIO_ENDPOINT, (uint8_t*)wav_buff16, sizeof(wav_buff16));
     }

    return;
}
