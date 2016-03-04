/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
 * Modifications (c) Copyright 2013 Ben Nahill
 * ALL RIGHTS RESERVED.
 *
 **************************************************************************//*!
 *
 * @file usb_descriptor.c
 *
 * @author
 *
 * @version
 *
 * @date May-28-2009
 *
 * @brief The file contains USB descriptors for Keyboard Application
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "derivative.h"
#include "types.h"
#include "usb_class.h"
#include "usb_descriptor.h"
#include "usb_framework.h"
#include "usb_composite.h"

#if (defined __MCF52xxx_H__)||(defined __MK_xxx_H__)
/* Put CFV2 and Kinetis descriptors in RAM */
#define USB_DESC_CONST
#else
#define USB_DESC_CONST	const
#endif

/*****************************************************************************
 * Constant and Macro's
 *****************************************************************************/

#define AUDIO_CC                           (0x01)
#define CDC_CC                             (0x02)
#define HID_CC                             (0x03)
#define MSD_CC                             (0x08)
#define VIDEO_CC                           (0x0e)
#define PHDC_CC                            (0x0f)
#define DFU_CC                             (0xfe)


/* structure containing details of all the endpoints used by this device */
/*
const USB_ENDPOINTS usb_desc_ep = {
				HID_DESC_ENDPOINT_COUNT,
				{
						HID_ENDPOINT,
						USB_INTERRUPT_PIPE,
						USB_SEND,
						HID_ENDPOINT_PACKET_SIZE,
				}
};
*/

uint_8 USB_DESC_CONST g_device_descriptor[DEVICE_DESCRIPTOR_SIZE] =
{
    DEVICE_DESCRIPTOR_SIZE,               /*  Device Descriptor Size        */
    USB_DEVICE_DESCRIPTOR,                /*  Device  Type of descriptor    */
    0x00, 0x02,                           /*  BCD USB version               */
    0x00,                                 /*  Device Class is indicated in
                                              the interface descriptors     */
    0x00,                                 /*  Device Subclass is indicated
                                              in the interface descriptors  */
    0x00,                                 /*  Device Protocol               */
    CONTROL_MAX_PACKET_SIZE,              /*  Max Packet size               */
    0xA2,0x15,                            /*  Vendor ID                     */
    0xEF,0xBE,                            /*  Product ID (0x0101 for KBD)   */
    0x00,0x03,                            /*  BCD Device version            */
    0x01,                                 /*  Manufacturer string index     */
    0x02,                                 /*  Product string index          */
    0x00,                                 /*  Serial number string index    */
    0x01                                  /*  Number of configurations      */
}; // Ben concludes that this looks A-okay



/* structure containing details of all the endpoints used by this device */
uint_8 usb_hid_class_info[] =
{
    /* Class type */
    HID_CC,
    /* Endpoint count */
    2,
    /* Hid endpoint in */
    HID_ENDPOINT_IN,
    USB_INTERRUPT_PIPE,
    USB_SEND,
    USB_uint_16_high(HID_ENDPOINT_PACKET_SIZE),
    USB_uint_16_low(HID_ENDPOINT_PACKET_SIZE),
    
    HID_ENDPOINT_OUT,
    USB_INTERRUPT_PIPE,
    USB_RECV,
    USB_uint_16_high(HID_ENDPOINT_PACKET_SIZE),
    USB_uint_16_low(HID_ENDPOINT_PACKET_SIZE),
	// As far as I can tell, the below is never accessed
    /* Interface count */
    1,
    /* Interface number */
	2,
};

#define CDC_DESC_ENDPOINT_COUNT       (2)
#define DIC_BULK_IN_ENDPOINT          (1)
#define DIC_BULK_OUT_ENDPOINT         (2)
#define DIC_BULK_IN_ENDP_PACKET_SIZE  (64)
#define DIC_BULK_OUT_ENDP_PACKET_SIZE (DIC_BULK_IN_ENDP_PACKET_SIZE)

uint_8 usb_cdc_class_info[] =
{
	/* Class type */
	CDC_CC,
	/* Endpoint count */
	CDC_DESC_ENDPOINT_COUNT,
	/* Cdc endpoint */
	DIC_BULK_IN_ENDPOINT,
	USB_BULK_PIPE,
	USB_SEND,
	0,
	0,
	USB_uint_16_high(DIC_BULK_IN_ENDP_PACKET_SIZE),
	USB_uint_16_low(DIC_BULK_IN_ENDP_PACKET_SIZE),
	DIC_BULK_OUT_ENDPOINT,
	USB_BULK_PIPE,
	USB_RECV,
	0,
	0,
	USB_uint_16_high(DIC_BULK_OUT_ENDP_PACKET_SIZE),
	USB_uint_16_low(DIC_BULK_OUT_ENDP_PACKET_SIZE),
	/* Interface count */
	2,
	/* Interface number */
	0,
	2,
};

uint_8 usb_audio_class_info[] =
{
    /* Class type */
    AUDIO_CC,
    /* Endpoint count */
    1,
    /* Audio endpoint */
    AUDIO_ENDPOINT,
    USB_ISOCHRONOUS_PIPE,
    USB_SEND,
    USB_uint_16_high(AUDIO_ENDPOINT_PACKET_SIZE),
    USB_uint_16_low(AUDIO_ENDPOINT_PACKET_SIZE),
	// As far as I can tell, the below is never accessed
    /* Interface count */
    1,
    /* Interface number */
    1,
};

DEV_ARCHITECTURE_STRUCT usb_dev_arc =
{
    COMP_CLASS_UNIT_COUNT,
    COMPOSITE_DESC_ENDPOINT_COUNT,
    {
		//(uint_8 *)&usb_hid_class_info,
		(uint_8 *)&usb_cdc_class_info,
        (uint_8 *)&usb_audio_class_info,
    }

};



/* Struct of Input Terminal, Output Terminal or Feature Unit */
USB_DESC_CONST USB_AUDIO_UNITS usb_audio_unit =
{
    AUDIO_UNIT_COUNT,
    {
        //{0x01,AUDIO_CONTROL_INPUT_TERMINAL},
        //{0x02,AUDIO_CONTROL_FEATURE_UNIT},
        //{0x03,AUDIO_CONTROL_OUTPUT_TERMINAL},
		{AUDIO_CONTROL_CLOCK_SOURCE_ID,   AUDIO_CONTROL_CLOCK_SOURCE},
        {AUDIO_CONTROL_INPUT_TERMINAL_ID, AUDIO_CONTROL_INPUT_TERMINAL}, 
        {AUDIO_CONTROL_FEATURE_UNIT_ID,   AUDIO_CONTROL_FEATURE_UNIT},
    }
};

uint_8 USB_DESC_CONST g_config_descriptor[] =
{
	/* Configuration Descriptor */
	CONFIG_ONLY_DESC_SIZE,               /* Size of this desciptor in bytes */
	USB_CONFIG_DESCRIPTOR,               /* DEVICE descriptor */
	/* Length of total configuration block */
	USB_uint_16_low(CONFIG_DESC_SIZE), USB_uint_16_high(CONFIG_DESC_SIZE),
	USB_MAX_SUPPORTED_INTERFACES,                                /* This device has two interface */
	// 4 total: audio ctl, audio stream, alt (unused) audio stream, and HID, but only 3 used
	0x01,                                /* ID of this configuration */
	0x00,                                /* Unused */
	0x80,                                /* Bus-powered device, no remote wakeup */
	0xFA,                                /* 500mA maximum power consumption */

	// This part looks pretty good!




	/*****************************************************************
	*            AUDIO FUNCTION DESCRIPTOR                           *
	******************************************************************/
	/* Standard Interface Association Descriptor */
	0x08,                      /* bLength(0x08) */
	0x0B,                      /* bDescriptorType(0x0B) */
	0x00,                      /* bFirstInterface(0x00) */
	0x02,                      /* bInterfaceCount(0x02) */
	0x01,                      /* bFunctionClass(0x01): AUDIO */
	0x00,                      /* bFunctionSubClass(0x00) */
	0x20,                      /* bFunctionProtocol(0x2000): 2.0 AF_VERSION_02_00 */
	0x00,                      /* iFunction(0x00) */
	
	/* AUDIO CONTROL INTERFACE DISCRIPTOR */
	IFACE_ONLY_DESC_SIZE,      /* Size of this descriptor - bLength (9) */
	USB_IFACE_DESCRIPTOR,      /* INTERFACE descriptor */
	0x00,                      /* Index of this interface */
	0x00,                      /* Index of this setting */
	0x00,                      /* 0 endpoint */
	USB_DEVICE_CLASS_AUDIO,    /* AUDIO */
	USB_SUBCLASS_AUDIOCONTROL, /* AUDIO_CONTROL */
	0x20,                      /* bInterfaceProtocol(0x20): IP 2.0 IP_VERSION_02_00 */
	0x07,                      /* iInterface(0x07): Not Requested */

	/* Audio class-specific interface header */
	HEADER_ONLY_DESC_SIZE,           /* bLength (9) */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
	AUDIO_CONTROL_HEADER,            /* bDescriptorSubtype (HEADER) */
	0x00,0x02,                       /* bcdADC (2.0) */
	0x03,                            /* bCategory(0x03): Microphone */
	0x3C,0x00,                       /* wTotalLength (60)  STARTING WITH HEADER */
	0x00,                            /* bmControls(0b00000000) */
	
	/* Clock Source Descriptor(4.7.2.1) */
	0x08, /* bLength(0x08) */
	0x24, /* bDescriptorType(0x24): CS_INTERFACE */
	AUDIO_CONTROL_CLOCK_SOURCE, /* bDescriptorSubType(0x0A): CLOCK_SOURCE */
	AUDIO_CONTROL_CLOCK_SOURCE_ID, /* bClockID(0x10): CLOCK_SOURCE_ID */
	0x01, /* bmAttributes(0x01): internal fixed clock */
	0x05, /* bmControls(0x07):
	         clock frequency control: 0b01 - host read only;
	         clock validity control: 0b01 - host read only */
	0x00, /* bAssocTerminal(0x00) */
	0x01, /* iClockSource(0x01): Not requested */

	// Ben says check that size!

	/* Audio class-specific input terminal */
	0x11,                            /* bLength (17) */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
	AUDIO_CONTROL_INPUT_TERMINAL,    /* bDescriptorSubtype (INPUT_TERMINAL) */
	AUDIO_CONTROL_INPUT_TERMINAL_ID, /* bTerminalID (0x20): INPUT_TERMINAL_ID */
	0x01,0x01,                       /* wTerminalType (USB streaming) */
	0x00,                            /* bAssocTerminal (none) */
	AUDIO_CONTROL_CLOCK_SOURCE_ID,   /* bCSouceID(0x10): CLOCK_SOURCE_ID */
	0x01,                            /* bNrChannels (2) */
	0x00,0x00,0x00,0x00,             /* bmChannelConfig (0x00): Mono, no spatial */
	0x00,                            /* iChannelNames (none) */
	0x00, 0x00,                      /* bmControls (0x0000) */
	0x02,                            /* iTerminal(0x02): not requested */


	/* Audio class-specific feature unit */
	0x0E,                            /* bLength (0x0E): 6 + (ch + 1) * 4, 1 channel */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
	0x06,                            /* bDescriptorSubtype (FEATURE_UNIT) */
	AUDIO_CONTROL_FEATURE_UNIT_ID,   /* bUnitID (0x30): FEATURE_UNIT_ID */
	AUDIO_CONTROL_INPUT_TERMINAL_ID, /* bSourceID (INPUT_TERMINAL_ID) */
	0x0F, 0x00, 0x00, 0x00, /* bmaControls(0)(0x0000000F): Master Channel 0
	                           0b11: Mute read/write
	                           0b11: Volume read/write */
	0x00, 0x00, 0x00, 0x00, /* bmaControls(1)(0x00000000): Logical Channel 1 */
	0x00,                   /* iFeature(0x00) */

	/* Audio class-specific output terminal */
	0x0C,                            /* bLength (12) */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
	AUDIO_CONTROL_OUTPUT_TERMINAL,   /* bDescriptorSubtype (OUTPUT_TERMINAL) */
	0x40,                            /* bTerminalID (0x40) */
	0x01,0x01,                       /* wTerminalType (USB streaming) */
	0x00,                            /* bAssocTerminal (None) */
	AUDIO_CONTROL_FEATURE_UNIT_ID,   /* bSourceID(0x30): FEATURE_UNIT_ID */
	AUDIO_CONTROL_CLOCK_SOURCE_ID,   /* bCSouceID(0x10): CLOCK_SOURCE_ID */
	0x00, 0x00,                      /* bmControls(0x0000) */
	0x00,                            /* iTerminal(0x00) */

	/* USB speaker standard AS interface descriptor - audio streaming operational
	   (Interface 3, Alternate Setting 0) */
	IFACE_ONLY_DESC_SIZE,            /* bLength (9) */
	USB_IFACE_DESCRIPTOR,            /* bDescriptorType (CS_INTERFACE) */
	0x01,                            /* interface Number: 1 */
	0x00,                            /* Alternate Setting: 0 */
	0x00,                            /* not used (Zero Bandwidth) */
	USB_DEVICE_CLASS_AUDIO,          /* USB DEVICE CLASS AUDIO */
	USB_SUBCLASS_AUDIOSTREAM,        /* AUDIO SUBCLASS AUDIOSTREAMING */
	0x20,                            /* AUDIO PROTOCOL IP 2.0 */
	//0x00,                            // bInterfaceProtocol should be 0x00 according to USB
	0x00,                            /* iInterface */

	/* USB speaker standard AS interface descriptor - audio streaming operational
	   (Interface 3, Alternate Setting 1) */
	IFACE_ONLY_DESC_SIZE,            /* bLength (9) */
	USB_IFACE_DESCRIPTOR,            /* bDescriptorType (CS_INTERFACE) */
	0x01,                            /* interface Number: 1 */
	0x01,                            /* Alternate Setting: 1 */
	0x01,                            /* One Endpoint. */
	USB_DEVICE_CLASS_AUDIO,          /* USB DEVICE CLASS AUDIO */
	USB_SUBCLASS_AUDIOSTREAM,        /* AUDIO SUBCLASS AUDIOSTREAMING */
	0x20,                            /* AUDIO PROTOCOL IP 2.0 */
	//0x00,                            // bInterfaceProtocol should be 0x00 according to USB
	0x00,                            /* iInterface */

	/* USB speaker standard General AS interface descriptor */
	0x10,                            /* bLength (16) */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
	AUDIO_STREAMING_GENERAL,         /* GENERAL subtype  */
	AUDIO_CONTROL_INPUT_TERMINAL_ID, /* bTerminalLink(0x20): INPUT_TERMINAL_ID */
	0x00,                            /* bmControls(0x00) */
	0x01,                            /* bFormatType(0x01): FORMAT_TYPE_I */
	0x01, 0x00, 0x00, 0x00,          /* bmFormats(0x00000001): PCM */
	0x01,                            /* bNrChannels(0x01): NB_CHANNELS */
	0x01, 0x00, 0x00, 0x00,          /* bmChannelConfig(0x00000001) */
	0x00,                            /* iChannelNames(0x00): None */

	/* USB speaker audio type I format interface descriptor */
	0x06,                            /* bLength (6) */
	AUDIO_INTERFACE_DESCRIPTOR_TYPE, /* bDescriptorType (CS_INTERFACE) */
	AUDIO_STREAMING_FORMAT_TYPE,     /* DescriptorSubtype: AUDIO STREAMING FORMAT TYPE */
	AUDIO_FORMAT_TYPE_I,             /* Format Type: Type I */
#if AUDIO_ENDPOINT_SAMPLE_SIZE==4
	0x04,                            /* SubFrame Size: 4 bytes per audio subframe */
	0x20,                            /* Bit Resolution: 32 bits per sample */
#elif AUDIO_ENDPOINT_SAMPLE_SIZE==3
	0x03,                            /* SubFrame Size: 3 bytes per audio subframe */
	0x18,                            /* Bit Resolution: 24 bits per sample */
#elif AUDIO_ENDPOINT_SAMPLE_SIZE==2
    0x02,                            /* SubFrame Size: 2 bytes per audio subframe */
    0x10,                            /* Bit Resolution: 16 bits per sample */
#elif AUDIO_ENDPOINT_SAMPLE_SIZE==1
    0x01,                            /* SubFrame Size: 1 byte per audio subframe */
    0x08,                            /* Bit Resolution: 8 bits per sample */
#else
	#error "dumb sample size"
#endif

	/*Endpoint 1 - standard descriptor*/
	0x07,                            /* bLength (7) */
	USB_ENDPOINT_DESCRIPTOR,         /* Descriptor type (endpoint descriptor) */
	AUDIO_ENDPOINT|(USB_SEND << 7),  /* OUT endpoint address 1 */
	0x05,                            /* Asynchronous endpoint */
	//0x0D,                            // Synchronous endpoint
#if AUDIO_ENDPOINT_PACKET_SIZE==768
    0x00,0x03,                       /* 512 bytes */
#elif AUDIO_ENDPOINT_PACKET_SIZE==512
    0x00,0x02,                       /* 512 bytes */
#elif AUDIO_ENDPOINT_PACKET_SIZE==256
	0x00,0x01,                       /* 256 bytes */
#elif AUDIO_ENDPOINT_PACKET_SIZE==128
	0x80,0x00,                       /* 128 bytes */
#elif AUDIO_ENDPOINT_PACKET_SIZE==64
	0x40,0x00,                       /* 64 bytes */
#else
	#error "dumb endpoint size"
#endif
	AUDIO_ENDPOINT_PACKET_PERIOD,    // in 2^x ms

    /* Endpoint 1 - Audio streaming descriptor */
    0x08,                            /* bLength (8) */
    USB_AUDIO_DESCRIPTOR,            /* AUDIO ENDPOINT DESCRIPTOR TYPE */
    AUDIO_ENDPOINT_GENERAL,          /* AUDIO ENDPOINT GENERAL */
	0x80,                            /* bmAttributes: 0x00 */
	0x00,                            /* bmControls: 0x00 */
	0x01,                            /* bLockDelayUnits (ms) */
	0x80,0x00,                       /* wLockDelay (128) */


	// CDC Function Descriptor

	IFACE_ONLY_DESC_SIZE,
	USB_IFACE_DESCRIPTOR,
	2,    // bInterfaceNumber
	0x00, // bAlternateSetting
	2,    // Number of endpoints
	0x0A, // DATA Interface Class
	0x00, // Data Interface SubClass Code
	NO_CLASS_SPECIFIC_PROTOCOL,
	0x00, // Interface Description String Index

	// Endpoint descriptor
	ENDP_ONLY_DESC_SIZE,
	USB_ENDPOINT_DESCRIPTOR,
	DIC_BULK_IN_ENDPOINT|(USB_SEND << 7),
	USB_BULK_PIPE,
	DIC_BULK_IN_ENDP_PACKET_SIZE, 0x00,
	0x00, // This value is ignored for Bulk ENDPOINT

	// Endpoint descriptor
	ENDP_ONLY_DESC_SIZE,
	USB_ENDPOINT_DESCRIPTOR,
	DIC_BULK_OUT_ENDPOINT|(USB_RECV << 7),
	USB_BULK_PIPE,
	DIC_BULK_OUT_ENDP_PACKET_SIZE, 0x00,
	0x00, // This value is ignored for Bulk ENDPOINT


/*
	// HID FUNCTION DESCRIPTION

	// Interface Descriptor
    IFACE_ONLY_DESC_SIZE,
    USB_IFACE_DESCRIPTOR,
    0x02,
    0x00,
    HID_DESC_ENDPOINT_COUNT,
    0x03,
    0x00,
    0x00,
    0x00,

	// HID descriptor
    HID_ONLY_DESC_SIZE,
    USB_HID_DESCRIPTOR,
    0x00,0x01,
    0x00,
    0x01,
    0x22,
    USB_uint_16_low(REPORT_DESC_SIZE),USB_uint_16_high(REPORT_DESC_SIZE),

    // HID IN Endpoint descriptor
    ENDP_ONLY_DESC_SIZE,               // bLength (7)
    USB_ENDPOINT_DESCRIPTOR,           // bDescriptorType (ENDPOINT)
    HID_ENDPOINT_IN | (USB_SEND << 7), // bEndpointAddress (EP1 in)
    USB_INTERRUPT_PIPE,                // bmAttributes (interrupt)
    HID_ENDPOINT_PACKET_SIZE, 0x00,    // wMaxPacketSize (64)
    0x0A,                              // bInterval (10 milliseconds)
    
    // HID OUT Endpoint descriptor
    ENDP_ONLY_DESC_SIZE,             // bLength (7)
    USB_ENDPOINT_DESCRIPTOR,             // bDescriptorType (ENDPOINT)
    HID_ENDPOINT_OUT | (USB_RECV << 7),   // bEndpointAddress (EP2 out)
    USB_INTERRUPT_PIPE,             // bmAttributes (interrupt)
    HID_ENDPOINT_PACKET_SIZE, 0x00,           // wMaxPacketSize (64)
    0x0A,              // bInterval (1 millisecond)
*/
};


uint_8 USB_DESC_CONST g_report_descriptor[REPORT_DESC_SIZE] =
{
	0x06, 0x00, 0xff, // USAGE_PAGE (Vendordefined 0xFF00) Freescale 0x01, 0xff
	0x09, 0x01, // USAGE (Vendor-defined 0x0001)
	0xa1, 0x01, // COLLECTION (Application)

	/*IN Report */
	0x85, 0x01, // Report ID
	0x09, 0x01, // USAGE (Vendor Usage 1 0x0001)
	0x15, 0x00, // LOGICAL_MINIMUM (0 for signed byte?)
	0x26, 0x00, 0xff, // LOGICAL_MAXIMUM (255 for signed byte?)
	0x75, 0x08, // REPORT_SIZE (8)
	0x95, 0x3f, // REPORT_COUNT (63) Freescale 0x40 (64)
	0x81, 0x02, // INPUT (Data,Var,Abs) Freescale 0x00 INPUT (Data, Ary, Abs)

	// OUT Report
	0x85, 0x01, // Report ID
	0x09, 0x01, // USAGE (Vendor Usage 1 0x0002)
	0x15, 0x00, // LOGICAL_MINIMUM (0)
	0x26, 0x00, 0xff, // LOGICAL_MAXIMUM (255)
	0x75, 0x08, // REPORT_SIZE (8)
	0x95, 0x3f, // REPORT_COUNT (63)         Freescale 0x40 (64)
	0x91, 0x02, // OUTPUT (Data,Ary,Abs) Freescale 0x00 INPUT (Data, Ary, Abs)
	0xc0        // END_COLLECTION
};


uint_8 USB_DESC_CONST USB_STR_0[USB_STR_0_SIZE+USB_STR_DESC_SIZE] =
                                    {sizeof(USB_STR_0),
                                     USB_STRING_DESCRIPTOR,
                                      0x09,
                                      0x04/*equiavlent to 0x0409*/
                                    };

uint_8 USB_DESC_CONST USB_STR_1[USB_STR_1_SIZE+USB_STR_DESC_SIZE]
                          = {  sizeof(USB_STR_1),
                               USB_STRING_DESCRIPTOR,
                               'B',0,
                               'R',0,
                               'N',0,
                               ' ',0,
                               'S',0,
                               'A',0,
                               'N',0,
                               'A',0,
                               ' ',0,
                               'A',0,
                               'U',0,
                               'D',0,
                               'I',0,
                               'O',0,
                               'P',0,
                               'U',0,
                               'L',0,
                               'S',0,
                               'E',0
                          };


uint_8 USB_DESC_CONST USB_STR_2[USB_STR_2_SIZE+USB_STR_DESC_SIZE]
                          = {  sizeof(USB_STR_2),
                               USB_STRING_DESCRIPTOR,
                               'O',0,
                               'A',0,
                               'E',0,
                               ' ',0,
                               'I',0,
                               'N',0,
                               'T',0,
                               'E',0,
                               'R',0,
                               'F',0,
                               'A',0,
                               'C',0,
                               'E',0,
                               ' ',0,
                               'R',0,
                               '3',0,
                               'A',0
                          };

uint_8 USB_DESC_CONST USB_STR_n[USB_STR_n_SIZE+USB_STR_DESC_SIZE]
                          = {  sizeof(USB_STR_n),
                               USB_STRING_DESCRIPTOR,
                               'B',0,
                               'A',0,
                               'D',0,
                               ' ',0,
                               'S',0,
                               'T',0,
                               'R',0,
                               'I',0,
                               'N',0,
                               'G',0,
                               ' ',0,
                               'I',0,
                               'N',0,
                               'D',0,
                               'E',0,
                               'X',0
                          };

uint8_t USB_DESC_CONST USB_STR_3[USB_STR_3_SIZE+USB_STR_DESC_SIZE] = {
	sizeof(USB_STR_3),
	USB_STRING_DESCRIPTOR,
	'A', 0,
	'B', 0,
	'c', 0
};


USB_PACKET_SIZE const g_std_desc_size[USB_MAX_STD_DESCRIPTORS+1] =
                                    {0,
                                     DEVICE_DESCRIPTOR_SIZE,
                                     CONFIG_DESC_SIZE,
                                     0, /* string */
                                     0, /* Interface */
                                     0, /* Endpoint */
                                     0, /* Device Qualifier */
                                     0, /* other speed config */
                                     REPORT_DESC_SIZE
                                    };

uint_8_ptr const g_std_descriptors[USB_MAX_STD_DESCRIPTORS+1] =
                                           {
                                              NULL,
                                              (uint_8_ptr)g_device_descriptor,
                                              (uint_8_ptr)g_config_descriptor,
                                              NULL, /* string */
                                              NULL, /* Interface */
                                              NULL, /* Endpoint */
                                              NULL, /* Device Qualifier */
                                              NULL, /* other speed config*/
                                              (uint_8_ptr)g_report_descriptor
                                            };

uint_8 const g_string_desc_size[USB_MAX_STRING_DESCRIPTORS+1] =
                                    {sizeof(USB_STR_0),
                                     sizeof(USB_STR_1),
                                     sizeof(USB_STR_2),
									 sizeof(USB_STR_3),
									 sizeof(USB_STR_n)
                                    };

uint_8_ptr const g_string_descriptors[USB_MAX_STRING_DESCRIPTORS+1] =
                                    {
                                        (uint_8_ptr const)USB_STR_0,
                                        (uint_8_ptr const)USB_STR_1,
                                        (uint_8_ptr const)USB_STR_2,
                                        (uint_8_ptr const)USB_STR_n
                                    };

USB_ALL_LANGUAGES g_languages = { USB_STR_0, sizeof(USB_STR_0),
                                    {
                                        (uint_16 const)0x0409,
                                        (const uint_8 **)g_string_descriptors,
                                        g_string_desc_size
                                    }
                                };

uint_8 const g_valid_config_values[USB_MAX_CONFIG_SUPPORTED+1]={0,1};

/****************************************************************************
 * Global Variables
 ****************************************************************************/
static uint_8 g_alternate_interface[USB_MAX_SUPPORTED_INTERFACES];

/*****************************************************************************
 * Local Types - None
 *****************************************************************************/

/*****************************************************************************
 * Local Functions Prototypes
 *****************************************************************************/

/*****************************************************************************
 * Local Variables - None
 *****************************************************************************/


 /*****************************************************************************
 * Local Functions - None
 *****************************************************************************/

/*****************************************************************************
 * Global Functions
 *****************************************************************************/
/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Descriptor
 *
 * @brief The function returns the correponding descriptor
 *
 * @param controller_ID : Controller ID
 * @param type          : type of descriptor requested
 * @param sub_type      : string index for string descriptor
 * @param index         : string descriptor language Id
 * @param descriptor    : output descriptor pointer
 * @param size          : size of descriptor returned
 *
 * @return USB_OK                      When Successfull
 *         USBERR_INVALID_REQ_TYPE     when Error
 *****************************************************************************
 * This function is used to pass the pointer to the requested descriptor
 *****************************************************************************/
uint_8 USB_Desc_Get_Descriptor (
    uint_8 controller_ID,   /* [IN] Controller ID */
    uint_8 type,            /* [IN] type of descriptor requested */
    uint_8 str_num,         /* [IN] string index for string descriptor */
    uint_16 index,          /* [IN] string descriptor language Id */
    uint_8_ptr *descriptor, /* [OUT] output descriptor pointer */
    USB_PACKET_SIZE *size   /* [OUT] size of descriptor returned */
)
{
    UNUSED (controller_ID)
    switch(type)
    {
      case USB_REPORT_DESCRIPTOR:
        {
          //__BKPT();
          type = USB_MAX_STD_DESCRIPTORS;
          *descriptor = g_report_descriptor;//(uint_8_ptr)g_std_descriptors [type];
          *size = REPORT_DESC_SIZE;// g_std_desc_size[type];

        }
        break;
      case USB_AUDIO_DESCRIPTOR:
        {
          type = USB_CONFIG_DESCRIPTOR ;
          *descriptor = (uint_8_ptr)(g_std_descriptors [type]+
                               CONFIG_ONLY_DESC_SIZE+IFACE_ONLY_DESC_SIZE);
          *size = HID_ONLY_DESC_SIZE;
        }
        break;
      case USB_HID_DESCRIPTOR:
        {
          type = USB_CONFIG_DESCRIPTOR ;
          *descriptor = (uint_8_ptr)(g_std_descriptors [type]+
                               CONFIG_ONLY_DESC_SIZE+IFACE_ONLY_DESC_SIZE);
          *size = HID_ONLY_DESC_SIZE;
        }
        break;
      case USB_STRING_DESCRIPTOR:
        {
            if(index == 0)
            {
                /* return the string and size of all languages */
                *descriptor = (uint_8_ptr)g_languages.
                                            languages_supported_string;
                *size = g_languages.languages_supported_size;
            } else
            {
                uint_8 lang_id=0;
                uint_8 lang_index=USB_MAX_LANGUAGES_SUPPORTED;

                for(;lang_id< USB_MAX_LANGUAGES_SUPPORTED;lang_id++)
                {
                    /* check whether we have a string for this language */
                    if(index == g_languages.usb_language[lang_id].language_id)
                    {   /* check for max descriptors */
                        if(str_num < USB_MAX_STRING_DESCRIPTORS)
                        {   /* setup index for the string to be returned */
                            lang_index=str_num;
                        }

                        break;
                    }

                }

                /* set return value for descriptor and size */
                *descriptor = (uint_8_ptr)g_languages.usb_language[lang_id].
                                            lang_desc[lang_index];
                *size = g_languages.usb_language[lang_id].
                                            lang_desc_size[lang_index];
            }
        }
        break;
      default :
        if (type < USB_MAX_STD_DESCRIPTORS)
        {
            /* set return val for descriptor and size*/
            *descriptor = (uint_8_ptr)g_std_descriptors [type];

            /* if there is no descriptor then return error */
            if(*descriptor == NULL)
            {
                return USBERR_INVALID_REQ_TYPE;
            }

            *size = g_std_desc_size[type];
        }
        else /* invalid descriptor */
        {
            return USBERR_INVALID_REQ_TYPE;
        }
        break;
    }
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Interface
 *
 * @brief The function returns the alternate interface
 *
 * @param controller_ID : Controller Id
 * @param interface     : Interface number
 * @param alt_interface : Output alternate interface
 *
 * @return USB_OK                     When Successfull
 *         USBERR_INVALID_REQ_TYPE    when Error
 *****************************************************************************
 * This function is called by the framework module to get the current interface
 *****************************************************************************/
uint_8 USB_Desc_Get_Interface (
    uint_8 controller_ID,   /* [IN] Controller ID */
    uint_8 interface,       /* [IN] interface number */
    uint_8_ptr alt_interface  /* [OUT] output alternate interface */
)
{
    UNUSED (controller_ID)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* get alternate interface*/
        *alt_interface = g_alternate_interface[interface];
        return USB_OK;
    }

    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Interface
 *
 * @brief The function sets the alternate interface
 *
 * @param controller_ID : Controller Id
 * @param interface     : Interface number
 * @param alt_interface : Input alternate interface
 *
 * @return USB_OK                              When Successfull
 *         USBERR_INVALID_REQ_TYPE             when Error
 *****************************************************************************
 * This function is called by the framework module to set the interface
 *****************************************************************************/
uint_8 USB_Desc_Set_Interface (
    uint_8 controller_ID, /* [IN] Controller ID */
    uint_8 interface,     /* [IN] interface number */
    uint_8 alt_interface  /* [IN] input alternate interface */
)
{
    UNUSED (controller_ID)
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES)
    {
        /* set alternate interface*/
        g_alternate_interface[interface]=alt_interface;
        return USB_OK;
    }

    while(1);
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Valid_Configation
 *
 * @brief The function checks whether the configuration parameter
 *        input is valid or not
 *
 * @param controller_ID : Controller Id
 * @param config_val    : Configuration value
 *
 * @return TRUE           When Valid
 *         FALSE          When Error
 *****************************************************************************
 * This function checks whether the configuration is valid or not
 *****************************************************************************/
boolean USB_Desc_Valid_Configation (
    uint_8 controller_ID,/*[IN] Controller ID */
    uint_16 config_val   /*[IN] configuration value */
)
{
    uint_8 loop_index=0;
    UNUSED (controller_ID)

    /* check with only supported val right now */
    while(loop_index < (USB_MAX_CONFIG_SUPPORTED+1))
    {
        if(config_val == g_valid_config_values[loop_index])
        {
            return TRUE;
        }
        loop_index++;
    }

    return FALSE;
}
/**************************************************************************//*!
 *
 * @name  USB_Desc_Valid_Interface
 *
 * @brief The function checks whether the interface parameter
 *        input is valid or not
 *
 * @param controller_ID : Controller Id
 * @param interface     : Target interface
 *
 * @return TRUE           When Valid
 *         FALSE          When Error
 *****************************************************************************
 * This function checks whether the interface is valid or not
 *****************************************************************************/
boolean USB_Desc_Valid_Interface (
    uint_8 controller_ID,   /*[IN] Controller ID */
    uint_8 interface        /*[IN] target interface */
)
{
    uint_8 loop_index=0;
    UNUSED (controller_ID)
    /* check with only supported val right now */
    while(loop_index < USB_MAX_SUPPORTED_INTERFACES)
    {
        if(interface == g_alternate_interface[loop_index])
        {
            return TRUE;
        }
        loop_index++;
    }

    return FALSE;
}
/**************************************************************************//*!
 *
 * @name  USB_Desc_Remote_Wakeup
 *
 * @brief The function checks whether the remote wakeup is supported or not
 *
 * @param controller_ID : Controller ID
 *
 * @return REMOTE_WAKEUP_SUPPORT (TRUE) - if remote wakeup supported
 *****************************************************************************
 * This function returns remote wakeup is supported or not
 *****************************************************************************/
boolean USB_Desc_Remote_Wakeup (
    uint_8 controller_ID  /* [IN] Controller ID */
)
{
    UNUSED (controller_ID)
    return REMOTE_WAKEUP_SUPPORT;
}




/**************************************************************************//*!
*
* @name  USB_Desc_Get_Endpoints
*
* @brief The function returns with the list of all non control endpoints used
*
* @param controller_ID : Controller ID
*
* @return pointer to USB_ENDPOINTS
*****************************************************************************
* This function returns the information about all the non control endpoints
* implemented
*****************************************************************************/
static USB_ENDPOINTS desc_ep_buffer[4];

USB_ENDPOINTS_PTR usb_desc_ep = NULL;
void* USB_Desc_Get_Endpoints(
uint_8 controller_ID      /* [IN] Controller ID */
)
{
    static uint8_t buffer_count = 0;
    uint_8 count,cl_count;
    uint_8 index = 0, offset = 0;
    ENDPOINT_ARC_STRUCT_PTR ep_arc_ptr = NULL;
    CLASS_ARC_STRUCT_PTR dev_class_ptr = NULL;
    UNUSED (controller_ID);

    if(usb_desc_ep == NULL)
    {
        //usb_desc_ep = (USB_ENDPOINTS_PTR)malloc(sizeof(USB_ENDPOINTS));
	usb_desc_ep = &desc_ep_buffer[buffer_count];
	
	buffer_count = (buffer_count + 1) & 3;
	if(!usb_desc_ep){
	  while(1);
	}

        usb_desc_ep->count = COMPOSITE_DESC_ENDPOINT_COUNT;
        for(cl_count = 0; cl_count < usb_dev_arc.cl_count; cl_count++)
        {
            dev_class_ptr = (CLASS_ARC_STRUCT_PTR)usb_dev_arc.value[cl_count];
            ep_arc_ptr = (ENDPOINT_ARC_STRUCT_PTR)&dev_class_ptr->value[0];
            for(count = 0; count < ep_arc_ptr->ep_count; count++)
            {
                USB_EP_STRUCT_PTR ep_struct_ptr = (USB_EP_STRUCT_PTR)&ep_arc_ptr->value[offset];

                usb_desc_ep->ep[index].ep_num = ep_struct_ptr->ep_num;
                usb_desc_ep->ep[index].type = ep_struct_ptr->type;
                usb_desc_ep->ep[index].direction = ep_struct_ptr->direction;
                usb_desc_ep->ep[index].size = ieee_htons(ep_struct_ptr->size);
                offset += sizeof(USB_EP_STRUCT);
                index ++;
            }
            offset = 0;
        }
    }
    return (void*)usb_desc_ep;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Entities
*
* @brief The function returns with the list of all Input Terminal,
* Output Terminal and Feature Unit
*
* @param controller_ID : Controller ID
*
* @return pointer to USB_VIDEO_UNITS
*****************************************************************************
* This function returns the information about all the Input Terminal,
* Output Terminal and Feature Unit
*****************************************************************************/
void* USB_Desc_Get_Audio_Entities(
uint_8 controller_ID      /* [IN] Controller ID */
)
{
    UNUSED (controller_ID);
    return (void*)&usb_audio_unit;
}


uint_8 g_cur_brightness[2] = {0x00,0x00};
/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Brightness
*
* @brief The function sets current brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
*Set current brightness value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Brightness(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED(controller_ID);
    UNUSED(size);

    g_cur_brightness[0]=**data;
    g_cur_brightness[1]=*(*data+1);

    return USB_OK;
}
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Brightness
*
* @brief The function gets the current Brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return current brightness value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Brightness(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    /*.Body*/
    UNUSED(controller_ID);

    *size = 2;
    *data = g_cur_brightness;

    return USB_OK;
}/*EndBody*/

uint_8 g_min_brightness[2] = {0xf0,0xff};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Min_Brightness
*
* @brief The function gets the minimum Brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return minimum brightness value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Min_Brightness(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    /*.Body*/
    UNUSED(controller_ID);

    *size = 2;
    *data = g_min_brightness;

    return USB_OK;
}/*EndBody*/
uint_8 g_max_brightness[2] = {0x10,0x00};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Max_Brightness
*
* @brief The function gets the maximum Brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return maximum brightness value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Max_Brightness(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    /*.Body*/
    UNUSED(controller_ID);

    *size = 2;
    *data = g_max_brightness;

    return USB_OK;
}/*EndBody*/

uint_8 g_res_brightness[2] = {0x01,0x00};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Res_Brightness
*
* @brief The function gets the resolution Brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return resolution brightness value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Res_Brightness(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    /*.Body*/
    UNUSED(controller_ID);

    *size = 2;
    *data = g_res_brightness;

    return USB_OK;
}/*EndBody*/

uint_8 g_info_brightness[1] = {0x03};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Info_Brightness
*
* @brief The function gets the information Brightness value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return information brightness value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Info_Brightness(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    /*.Body*/
    UNUSED(controller_ID);

    *size = 1;
    *data = g_info_brightness;

    return USB_OK;
}/*EndBody*/

uint_8 g_def_brightness[2] = {0x01,0x00};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Def_Brightness
*
* @brief The function gets the default Brightness value
*
* @param controller_ID : Controller ID
* @param data             : Pointer to Data
* @param size             : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return default brightness value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Def_Brightness(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED(controller_ID);

    *size = 2;
    *data = g_def_brightness;

    return USB_OK;
}

uint_8 g_cur_power_mode[1] = {0x00};
/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Power_Mode
*
* @brief The function sets current power mode value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
*Set current power mode value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Power_Mode(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED(controller_ID);
    UNUSED(size);

    g_cur_power_mode[0]=**data;

    return USB_OK;
}
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Power_Mode
*
* @brief The function gets the current power mode value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return current power mode value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Power_Mode(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    /*.Body*/
    UNUSED(controller_ID);

    *size = 1;
    *data = g_cur_power_mode;
    return USB_OK;
}/*EndBody*/

uint_8 g_info_power_mode[1] = {0x03};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Info_Power_Mode
*
* @brief The function gets the information power mode value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return information brightness value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Info_Power_Mode(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED(controller_ID);

    *size = 1;
    *data = g_info_power_mode;
    return USB_OK;
}


uint_8 g_cur_error_code[1] = {0x07};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Error_Code
*
* @brief The function gets the current error code value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return current error code value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Error_Code(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED(controller_ID);

    *size = 1;
    *data = g_cur_error_code;
    return USB_OK;
}

uint_8 g_info_error_code[1] = {0x01};
/**************************************************************************//*!
*
* @name  USB_Desc_Get_Info_Error_Code
*
* @brief The function gets the information error code value
*
* @param controller_ID : Controller ID
* @param data          : Pointer to Data
* @param size          : Pointer to Size of Data
*
* @return USB_OK                              When Successfull
*         USBERR_INVALID_REQ_TYPE             when Error
*****************************************************************************
* Return information error code value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Info_Error_Code(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED(controller_ID);

    *size = 1;
    *data = g_info_error_code;
    return USB_OK;
}

/*************************************************************************
********************     AUDIO         **********************************
**************************************************************************/
uint_8 g_copy_protect[USB_MAX_SUPPORTED_INTERFACES]={0x01};

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Copy_Protect
*
* @brief  The function is called in response to Set Terminal Control Request
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data
* @param size            : Pointer to Size of Data
*
* @return  USB_OK                  : When Successfull
*          USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                    Interface is presented
******************************************************************************
* This function is called to Set Copy Protect Level
*****************************************************************************/
uint_8 USB_Desc_Set_Copy_Protect(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set copy protect data*/
        g_copy_protect[interface] = **data;
        return USB_OK;
    }

    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Copy_Protect
*
* @brief  The function is called in response to Get Terminal Control Request
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data
* @param size            : Pointer to Size of Data
*
* @return  USB_OK                  : When Successfull
*          USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                    Interface is presented
******************************************************************************
* This function is called to Get Copy Protect Level
*****************************************************************************/
uint_8 USB_Desc_Get_Copy_Protect(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get copy protect data*/
        *size=1;
        *data=&g_copy_protect[interface];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint_8 g_cur_mute[USB_MAX_SUPPORTED_INTERFACES] ={0x00};

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Mute
*
* @brief  The function sets Current Mute state
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data
* @param size            : Pointer to Size of Data
*
* @return    USB_OK                  : When Successfull
*            USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                      Interface is presented
******************************************************************************
* Sets current Mute state specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Mute (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data to be send */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set current mute data*/
        g_cur_mute[interface-SPEAKER_CTRL_INTF] = **data;

        #if (!(defined _MC9S08JS16_H))
        #endif

        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Mute
*
* @brief The function gets the current Mute state
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Mute state to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Mute(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get current mute data*/
        *size = 1;
        *data = &g_cur_mute[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint_8 g_cur_volume[USB_MAX_SUPPORTED_INTERFACES][2] = {0x00,0x80};
uint_8 g_min_volume[USB_MAX_SUPPORTED_INTERFACES][2] = {0x00,0x80};
uint_8 g_max_volume[USB_MAX_SUPPORTED_INTERFACES][2] = {0xFF,0x7F};
uint_8 g_res_volume[USB_MAX_SUPPORTED_INTERFACES][2] = {0x01,0x00};

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Volume
*
* @brief  The function sets Current Volume value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data
* @param size            : Pointer to Size of Data
*
* @return  USB_OK                  : When Successfull
*          USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                    Interface is presented
******************************************************************************
* Sets current Volume value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Volume (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    int_16 volume;
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set current volume data*/
        g_cur_volume[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_cur_volume[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);
        volume = (int_16)((g_cur_volume[interface-SPEAKER_CTRL_INTF][1] << 8) | g_cur_volume[interface-SPEAKER_CTRL_INTF][0]);
        /* The current volume is received as logical value
    * Logical  S16:  0x8000      to 0x7FFF             and corresponds to
    * Physical      -127.9661 dB to 127.9661 dB
    * The equation for the line that passes through the above coordinates results:
    *  y = 0.0039*x - 3.1
    */
#if (!(defined _MC9S08JS16_H))
        {
            int_16 volPhysical;
            if((uint_16)volume == 0x8000)
            {
#if USART_DEBUG
                (void)printf("Volume: Silence\r\n");
#endif /* USART_DEBUG */
            }
            else
            {
#if USART_DEBUG
                volPhysical = (int_16)((int_16)(((int_32)volume * 4) / 1000) - 3);
                (void)printf("Volume: %d dB\r\n", volPhysical);
#endif /* USART_DEBUG */
            }

        }
#endif
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Min_Volume
*
* @brief  The function sets Minimum Volume value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Minimum Volume value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Min_Volume(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data to send */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set min volume data*/
        g_min_volume[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_min_volume[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Set_Max_Volume
*
* @brief  The function sets Maximum Volume value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Maximum Volume value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Max_Volume (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set max volume data*/
        g_max_volume[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_max_volume[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Res_Volume
*
* @brief  The function sets Resolution Volume value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Resolution Volume value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Res_Volume(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set resolution volume data*/
        g_res_volume[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_res_volume[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Volume
*
* @brief  The function gets the Current Volume value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Current Volume value to the Host
*****************************************************************************/

uint_8 USB_Desc_Get_Cur_Volume(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get current volume data*/
        *size = 2;
        *data = g_cur_volume[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Min_Volume
*
* @brief  The function gets the Minimum Volume value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Min Volume value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Min_Volume(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get min volume data*/
        *size = 2;
        *data = g_min_volume[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Max_Volume
*
* @brief  The function gets the Maximum Volume value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Max Volume value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Max_Volume (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* Get max volume data*/
        *size = 2;
        *data = g_max_volume[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Res_Volume
*
* @brief  The function gets the Resolution Volume value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Resolution Volume value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Res_Volume(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* Get resolution volume data*/
        *size = 2;
        *data = g_res_volume[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint_8 g_cur_bass[USB_MAX_SUPPORTED_INTERFACES] = {0x00};
uint_8 g_min_bass[USB_MAX_SUPPORTED_INTERFACES] = {0x80};
uint_8 g_max_bass[USB_MAX_SUPPORTED_INTERFACES] = {0x7F};
uint_8 g_res_bass[USB_MAX_SUPPORTED_INTERFACES] = {0x01};

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Bass
*
* @brief  The function sets Current Bass value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Current Bass value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Bass (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set current bass data*/
        g_cur_bass[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Min_Bass
*
* @brief  The function sets Minimum Bass value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Minimum Bass value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Min_Bass(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set min bass data*/
        g_min_bass[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Max_Bass
*
* @brief  The function sets Maximum Bass value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Maximum Bass value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Max_Bass (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set max bass data*/
        g_max_bass[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Res_Bass
*
* @brief  The function sets Resolution Bass value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Resolution Bass value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Res_Bass (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set resolution bass data*/
        g_res_bass[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Bass
*
* @brief  The function gets the Current Bass value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Current Bass value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Bass (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get current bass data*/
        *size = 1;
        *data = &g_cur_bass[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Min_Bass
*
* @brief  The function gets the Minimum Bass value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Minimum Bass value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Min_Bass(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get min bass data*/
        *size = 1;
        *data = &g_min_bass[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Max_Bass
*
* @brief  The function gets the Maximum Bass value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Maximum Bass value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Max_Bass (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get max bass data*/
        *size = 1;
        *data = &g_max_bass[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }

    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Res_Bass
*
* @brief  The function gets the Resolution Bass value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Resolution Bass value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Res_Bass (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get resolution bass data*/
        *size = 1;
        *data = &g_res_bass[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint_8 g_cur_mid[USB_MAX_SUPPORTED_INTERFACES] = {0x00};
uint_8 g_min_mid[USB_MAX_SUPPORTED_INTERFACES] = {0x80};
uint_8 g_max_mid[USB_MAX_SUPPORTED_INTERFACES] = {0x7F};
uint_8 g_res_mid[USB_MAX_SUPPORTED_INTERFACES] = {0x01};

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Mid
*
* @brief  The function sets Current Mid value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Current Mid value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Mid(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set current mid data*/
        g_cur_mid[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Min_Mid
*
* @brief  This function is called to Set Minimum Mid value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Minumum Mid value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Min_Mid(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set min mid data*/
        g_min_mid[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Max_Mid
*
* @brief  This function is called to Set Maximum Mid value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Maximum Mid value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Max_Mid(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set max mid data*/
        g_max_mid[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Res_Mid
*
* @brief  This function is called to Set Resolution Mid value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Resolution Mid value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Res_Mid(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set resolution mid data*/
        g_res_mid[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Mid
*
* @brief  The function gets the Current Mid value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Current Mid value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Mid (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get current Mid data*/
        *size = 1;
        *data=&g_cur_mid[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Min_Mid
*
* @brief  The function gets the Minimum Mid value
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Minimum Mid value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Min_Mid (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface<USB_MAX_SUPPORTED_INTERFACES){
        /* get min mid data data*/
        *size = 1;
        *data = &g_min_mid[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Max_Mid
*
* @brief  The function gets the Maximum Mid value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Maximum Mid value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Max_Mid (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface<USB_MAX_SUPPORTED_INTERFACES){
        /* get max mid data data*/
        *size = 1;
        *data = &g_max_mid[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Res_Mid
*
* @brief  The function gets the Resolution Mid value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Resolution Mid value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Res_Mid (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface<USB_MAX_SUPPORTED_INTERFACES){
        /* get resolution mid data data*/
        *size = 1;
        *data = &g_res_mid[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint_8 g_cur_treble[USB_MAX_SUPPORTED_INTERFACES] = {0x01};
uint_8 g_min_treble[USB_MAX_SUPPORTED_INTERFACES] = {0x80};
uint_8 g_max_treble[USB_MAX_SUPPORTED_INTERFACES] = {0x7F};
uint_8 g_res_treble[USB_MAX_SUPPORTED_INTERFACES] = {0x01};

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Treble
*
* @brief  The function set Current Treble value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Current Treble value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Treble(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set current treble data*/
        g_cur_treble[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Min_Treble
*
* @brief  This function set Minimum Treble value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Minimum Treble value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Min_Treble(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set min treble data*/
        g_min_treble[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Max_Treble
*
* @brief  The function set Maximum Treble value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Maximum Treble value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Max_Treble(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set max treble data*/
        g_max_treble[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Res_Treble
*
* @brief  The function set Resolution Treble value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Resolution Treble value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Res_Treble(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set resolution treble data*/
        g_res_treble[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Treble
*
* @brief  The function get Current Treble value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Current Treble value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Treble (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get current treble data*/
        *size=1;
        *data=&g_cur_treble[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Min_Treble
*
* @brief  The function get Minimum Treble value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Minimum Treble value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Min_Treble (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get min treble data*/
        *size=1;
        *data=&g_min_treble[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Max_Treble
*
* @brief  The function get Maximum Treble value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Maximum Treble value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Max_Treble (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get max treble data*/
        *size=1;
        *data=&g_max_treble[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Res_Treble
*
* @brief  The function gets Resolution Treble value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Resolution Treble value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Res_Treble (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get resolution treble data*/
        *size=1;
        *data=&g_res_treble[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint_8 g_cur_graphic_equalizer[USB_MAX_SUPPORTED_INTERFACES][5] = {0x00,0x00,0x00,0x00,0x00};
uint_8 g_min_graphic_equalizer[USB_MAX_SUPPORTED_INTERFACES][5] = {0x00,0x00,0x00,0x00,0x00};
uint_8 g_max_graphic_equalizer[USB_MAX_SUPPORTED_INTERFACES][5] = {0x00,0x00,0x00,0x00,0x00};
uint_8 g_res_graphic_equalizer[USB_MAX_SUPPORTED_INTERFACES][5] = {0x00,0x00,0x00,0x00,0x00};

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Graphic_Equalizer
*
* @brief  The function sets Current Graphic Equalizer values
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Current Graphic Equalizer values specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Graphic_Equalizer(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set current graphic equalizer data*/
        g_cur_graphic_equalizer[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_cur_graphic_equalizer[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);
        g_cur_graphic_equalizer[interface-SPEAKER_CTRL_INTF][2] = *(*data+2);
        g_cur_graphic_equalizer[interface-SPEAKER_CTRL_INTF][3] = *(*data+3);
        g_cur_graphic_equalizer[interface-SPEAKER_CTRL_INTF][4] = *(*data+4);
        return USB_OK;
    }

    return USBERR_INVALID_REQ_TYPE;
}
/**************************************************************************//*!
*
* @name  USB_Desc_Set_Min_Graphic_Equalizer
*
* @brief  The function sets Minimum Graphic Equalizer values
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Minimum Graphic Equalizer values specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Min_Graphic_Equalizer(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set min graphic equalizer data*/
        g_min_graphic_equalizer[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_min_graphic_equalizer[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);
        g_min_graphic_equalizer[interface-SPEAKER_CTRL_INTF][2] = *(*data+2);
        g_min_graphic_equalizer[interface-SPEAKER_CTRL_INTF][3] = *(*data+3);
        g_min_graphic_equalizer[interface-SPEAKER_CTRL_INTF][4] = *(*data+4);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Max_Graphic_Equalizer
*
* @brief  The function sets Maximum Graphic Equalizer values
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Maximum Graphic Equalizer values specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Max_Graphic_Equalizer(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set max graphic equalizer data*/
        g_max_graphic_equalizer[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_max_graphic_equalizer[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);
        g_max_graphic_equalizer[interface-SPEAKER_CTRL_INTF][2] = *(*data+2);
        g_max_graphic_equalizer[interface-SPEAKER_CTRL_INTF][3] = *(*data+3);
        g_max_graphic_equalizer[interface-SPEAKER_CTRL_INTF][4] = *(*data+4);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Res_Graphic_Equalizer
*
* @brief  The function sets Resolution Graphic Equalizer values
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Resolution Graphic Equalizer values specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Res_Graphic_Equalizer(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set resolution graphic equalizer data*/
        g_res_graphic_equalizer[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_res_graphic_equalizer[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);
        g_res_graphic_equalizer[interface-SPEAKER_CTRL_INTF][2] = *(*data+2);
        g_res_graphic_equalizer[interface-SPEAKER_CTRL_INTF][3] = *(*data+3);
        g_res_graphic_equalizer[interface-SPEAKER_CTRL_INTF][4] = *(*data+4);
        return USB_OK;
    }

    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Graphic_Equalizer
*
* @brief  The function gets Current Graphic Equalizer values
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Current Graphic Equalizer values to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Graphic_Equalizer (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get current graphic equalizer data*/
        *size=5;
        *data=g_cur_graphic_equalizer[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Min_Graphic_Equalizer
*
* @brief  The function gets Minimum Graphic Equalizer values
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Minimum Graphic Equalizer values to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Min_Graphic_Equalizer (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get min graphic equalizer data*/
        *size=5;
        *data=g_min_graphic_equalizer[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Max_Graphic_Equalizer
*
* @brief  The function gets Maximum Graphic Equalizer values
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Maximum Graphic Equalizer values to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Max_Graphic_Equalizer (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{

    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get max graphic equalizer data*/
        *size=5;
        *data=g_max_graphic_equalizer[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Res_Graphic_Equalizer
*
* @brief  The function gets Resolution Graphic Equalizer values
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Resolution Graphic Equalizer values to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Res_Graphic_Equalizer (
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get resolution graphic equalizer data*/
        *size=5;
        *data=g_res_graphic_equalizer[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint_8 g_cur_automatic_gain[USB_MAX_SUPPORTED_INTERFACES] = {0x01};

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Automatic_Gain
*
* @brief  The function sets Current Automatic Gain value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Current Automatic Gain value specified by the Host
*****************************************************************************/

uint_8 USB_Desc_Set_Cur_Automatic_Gain(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set current automatic data*/
        g_cur_automatic_gain[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Automatic_Gain
*
* @brief  The function gets Current Automatic Gain value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Current Automatic Gain value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Automatic_Gain(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get current automatic data*/
        *size=1;
        *data=&g_cur_automatic_gain[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint_8 g_cur_delay[USB_MAX_SUPPORTED_INTERFACES][2] = {0x00,0x40};
uint_8 g_min_delay[USB_MAX_SUPPORTED_INTERFACES][2] = {0x00,0x00};
uint_8 g_max_delay[USB_MAX_SUPPORTED_INTERFACES][2] = {0xFF,0xFF};
uint_8 g_res_delay[USB_MAX_SUPPORTED_INTERFACES][2] = {0x00,0x01};
/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Delay
*
* @brief  The function sets Current Delay value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Set Current Delay value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Delay(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set current delay data*/
        g_cur_delay[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_cur_delay[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Min_Delay
*
* @brief  The function sets Minimum Delay value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Set Minimum Delay value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Min_Delay(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set min delay data*/
        g_min_delay[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_min_delay[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Max_Delay
*
* @brief  The function sets Maximum Delay value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Set Maximum Delay value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Max_Delay(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set max delay data*/
        g_max_delay[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_max_delay[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);

        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Res_Delay
*
* @brief  The function sets Resolution Delay value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Set Resolution Delay value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Res_Delay(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set resolution delay data*/
        g_res_delay[interface-SPEAKER_CTRL_INTF][0] = **data;
        g_res_delay[interface-SPEAKER_CTRL_INTF][1] = *(*data+1);
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Delay
*
* @brief  The function gets Current Delay value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Current Delay value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Delay(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get current delay data*/
        *size=2;
        *data=g_cur_delay[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Min_Delay
*
* @brief  The function gets Minimum Delay value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Minimum Delay value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Min_Delay(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get min delay data*/
        *size=2;
        *data=g_min_delay[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Max_Delay
*
* @brief  The function gets Maximum Delay value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Maximum Delay value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Max_Delay(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get max delay data*/
        *size=2;
        *data=g_max_delay[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Res_Delay
*
* @brief  The function gets Resolution Delay value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Resolution Delay value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Res_Delay(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get resolution delay data*/
        *size=2;
        *data=g_res_delay[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint_8 g_cur_bass_boost[USB_MAX_SUPPORTED_INTERFACES] = {0x01};

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Bass_Boost
*
* @brief  The function sets Current Bass Boost value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Set Current Bass Boost value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Bass_Boost(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set current bass boost data*/
        g_cur_bass_boost[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Bass_Boost
*
* @brief  The function gets Current Bass Boost value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Current Bass Boost value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Bass_Boost(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get current bass boost data*/
        *size=1;
        *data=&g_cur_bass_boost[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

uint_8 g_cur_loudness[USB_MAX_SUPPORTED_INTERFACES] = {0x01};

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Loudness
*
* @brief  The function sets Current Loudness value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Set Current Loudness value specified to the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Loudness(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* set current loudness data*/
        g_cur_loudness[interface-SPEAKER_CTRL_INTF] = **data;
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Loudness
*
* @brief  The function gets Current Loudness value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Current Loudness value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Loudness(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    /* if interface valid */
    if(interface < USB_MAX_SUPPORTED_INTERFACES){
        /* get current loudness data*/
        *size=1;
        *data=&g_cur_loudness[interface-SPEAKER_CTRL_INTF];
        return USB_OK;
    }
    return USBERR_INVALID_REQ_TYPE;
}




#if AUDIO_CLASS_2_0
uint_8 g_range_sampling_frequency[14] = {
	0x01, 0x00,
	0x80,0x3e,0x00,0x00,
	0x80,0x3e,0x00,0x00,
	0x00,0x00,0x01,0x00
};
uint_8 g_cur_sampling_frequency[4] = {0x80,0x3e,0x00,0x00};
uint_8 g_min_sampling_frequency[4] = {0x80,0x3e,0x00,0x00};
uint_8 g_max_sampling_frequency[4] = {0x80,0x3e,0x00,0x00};
uint_8 g_res_sampling_frequency[4] = {0x00,0x00,0x01,0x00};
#else
uint_8 g_range_sampling_frequency[11] = {
	0x01, 0x00,
	0x80,0x3e,0x00,
	0x80,0x3e,0x00,
	0x00,0x00,0x01
};
uint_8 g_cur_sampling_frequency[3] = {0x00,0x00,0x01};
uint_8 g_min_sampling_frequency[3] = {0x00,0x00,0x00};
uint_8 g_max_sampling_frequency[3] = {0x7F,0xFF,0xFF};
uint_8 g_res_sampling_frequency[3] = {0x00,0x01,0x00};
#endif /* AUDIO_CLASS_2_0 */

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Cur_Sampling_Frequency
 *
 * @brief  The function sets Current Sampling Frequency value
 *
 * @param controller_ID   : Controller ID
 * @param interface       : Interface
 * @param data            : Pointer to Data to be send
 * @param size            : Pointer to Size of Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Current Sampling Frequency value specified by the Host
 *****************************************************************************/
uint_8 USB_Desc_Set_Cur_Sampling_Frequency(
    uint_8 controller_ID,               /* [IN] Controller ID */
    uint_8 interface,                   /* [IN] Interface */
    uint_8_ptr *data,                   /* [IN] Pointer to Data */
    USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
) 
{
    UNUSED (controller_ID);
    UNUSED (size);
    UNUSED (interface);

    /* set current sampling fequency data*/
    g_cur_sampling_frequency[0] = **data;
    g_cur_sampling_frequency[1] = *(*data+1);
    g_cur_sampling_frequency[2] = *(*data+2);
#if AUDIO_CLASS_2_0
    g_cur_sampling_frequency[3] = *(*data+3);
#endif /* AUDIO_CLASS_2_0 */

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Min_Sampling_Frequency
 *
 * @brief  The function sets Minimum Sampling Frequency value
 *
 * @param controller_ID   : Controller ID
 * @param interface       : Interface
 * @param data            : Pointer to Data to be send
 * @param size            : Pointer to Size of Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Minimum Sampling Frequency value specified by the Host
 *****************************************************************************/
uint_8 USB_Desc_Set_Min_Sampling_Frequency(
    uint_8 controller_ID,               /* [IN] Controller ID */
    uint_8 interface,                   /* [IN] Interface */
    uint_8_ptr *data,                   /* [IN] Pointer to Data */
    USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
) 
{
    UNUSED (controller_ID);
    UNUSED (size);
    UNUSED (interface);
    /* set min sampling frequency data*/
    g_min_sampling_frequency[0] = **data;
    g_min_sampling_frequency[1] = *(*data+1);
    g_min_sampling_frequency[2] = *(*data+2);
#if AUDIO_CLASS_2_0
    g_min_sampling_frequency[3] = *(*data+3);
#endif /* AUDIO_CLASS_2_0 */

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Max_Sampling_Frequency
 *
 * @brief  The function sets Maximum Sampling Frequency value
 *
 * @param controller_ID   : Controller ID
 * @param interface       : Interface
 * @param data            : Pointer to Data to be send
 * @param size            : Pointer to Size of Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Maximum Sampling Frequency value specified by the Host
 *****************************************************************************/
uint_8 USB_Desc_Set_Max_Sampling_Frequency(
    uint_8 controller_ID,               /* [IN] Controller ID */
    uint_8 interface,                   /* [IN] Interface */
    uint_8_ptr *data,                   /* [IN] Pointer to Data */
    USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
) 
{
    UNUSED (controller_ID);
    UNUSED (size);
    UNUSED (interface);
    /* set max sampling frequency data*/
    g_max_sampling_frequency[0] = **data;
    g_max_sampling_frequency[1] = *(*data+1);
    g_max_sampling_frequency[2] = *(*data+2);
#if AUDIO_CLASS_2_0
    g_max_sampling_frequency[3] = *(*data+3);
#endif /* AUDIO_CLASS_2_0 */

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Set_Res_Sampling_Frequency
 *
 * @brief  The function sets Resolution Sampling Frequency value
 *
 * @param controller_ID   : Controller ID
 * @param interface       : Interface
 * @param data            : Pointer to Data to be send
 * @param size            : Pointer to Size of Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Sets Resolution Sampling Frequency value specified by the Host
 *****************************************************************************/
uint_8 USB_Desc_Set_Res_Sampling_Frequency(
    uint_8 controller_ID,               /* [IN] Controller ID */
    uint_8 interface,                   /* [IN] Interface */
    uint_8_ptr *data,                   /* [IN] Pointer to Data */
    USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
) 
{
    UNUSED (controller_ID);
    UNUSED (size);
    UNUSED (interface);
    /* set resolution sampling frequency data*/
    g_res_sampling_frequency[0] = **data;
    g_res_sampling_frequency[1] = *(*data+1);
    g_res_sampling_frequency[2] = *(*data+2);
#if AUDIO_CLASS_2_0
    g_cur_sampling_frequency[3] = *(*data+3);
#endif /* AUDIO_CLASS_2_0 */

    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Sampling_Frequency
 *
 * @brief  The function gets Current Sampling Frequency value
 *
 * @param controller_ID   : Controller ID
 * @param interface       : Interface
 * @param data            : Pointer to Data to be send
 * @param size            : Pointer to Size of Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Sampling Frequency value to the Host
 *****************************************************************************/
uint_8 USB_Desc_Get_Cur_Sampling_Frequency(
    uint_8 controller_ID,               /* [IN] Controller ID */
    uint_8 interface,                   /* [IN] Interface */
    uint_8_ptr *data,                   /* [OUT] Pointer to Data */
    USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
) 
{
    UNUSED (controller_ID);
    UNUSED (interface);
    /* get current sampling frequency data*/
#if AUDIO_CLASS_2_0
    *size=4;
#else
    *size=3;
#endif /* AUDIO_CLASS_2_0 */
    *data=g_cur_sampling_frequency;
    return USB_OK;
}

uint_8 USB_Desc_Get_Range_Sampling_Frequency(
    uint_8 controller_ID,               /* [IN] Controller ID */
    uint_8 interface,                   /* [IN] Interface */
    uint_8_ptr *data,                   /* [OUT] Pointer to Data */
    USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
) 
{
    UNUSED (controller_ID);
    UNUSED (interface);
    /* get current sampling frequency data*/
#if AUDIO_CLASS_2_0
    *size=sizeof(g_range_sampling_frequency);
#else
    *size=3;
#endif /* AUDIO_CLASS_2_0 */
    *data=g_range_sampling_frequency;
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Min_Sampling_Frequency
 *
 * @brief  The function gets Minimum Sampling Frequency value
 *
 * @param controller_ID   : Controller ID
 * @param interface       : Interface
 * @param data            : Pointer to Data to be send
 * @param size            : Pointer to Size of Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Minimum Sampling Frequency value to the Host
 *****************************************************************************/
uint_8 USB_Desc_Get_Min_Sampling_Frequency(
    uint_8 controller_ID,               /* [IN] Controller ID */
    uint_8 interface,                   /* [IN] Interface */
    uint_8_ptr *data,                   /* [OUT] Pointer to Data */
    USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
) 
{
    UNUSED (controller_ID);
    UNUSED (interface);
    /* get min sampling frequency data*/
#if AUDIO_CLASS_2_0
    *size=4;
#else
    *size=3;
#endif /* AUDIO_CLASS_2_0 */
    *data=g_min_sampling_frequency;
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Max_Sampling_Frequency
 *
 * @brief  The function gets Maximum Sampling Frequency value
 *
 * @param controller_ID   : Controller ID
 * @param interface       : Interface
 * @param data            : Pointer to Data to be send
 * @param size            : Pointer to Size of Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Maximum Sampling Frequency value to the Host
 *****************************************************************************/
uint_8 USB_Desc_Get_Max_Sampling_Frequency(
    uint_8 controller_ID,               /* [IN] Controller ID */
    uint_8 interface,                   /* [IN] Interface */
    uint_8_ptr *data,                   /* [OUT] Pointer to Data */
    USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
) 
{
    UNUSED (controller_ID);
    UNUSED (interface);
    /* get max sampling frequency data*/
#if AUDIO_CLASS_2_0
    *size=4;
#else
    *size=3;
#endif /* AUDIO_CLASS_2_0 */
    *data=g_max_sampling_frequency;
    return USB_OK;
}

/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Res_Sampling_Frequency
 *
 * @brief  The function gets Resolution Sampling Frequency value
 *
 * @param controller_ID   : Controller ID
 * @param interface       : Interface
 * @param data            : Pointer to Data to be send
 * @param size            : Pointer to Size of Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Resolution Sampling Frequency value to the Host
 *****************************************************************************/
uint_8 USB_Desc_Get_Res_Sampling_Frequency(
    uint_8 controller_ID,               /* [IN] Controller ID */
    uint_8 interface,                   /* [IN] Interface */
    uint_8_ptr *data,                   /* [OUT] Pointer to Data */
    USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
) 
{
    UNUSED (controller_ID);
    UNUSED (interface);
    /* get resolution sampling frequency data*/
#if AUDIO_CLASS_2_0
    *size=4;
#else
    *size=3;
#endif /* AUDIO_CLASS_2_0 */
    *data=g_res_sampling_frequency;
    return USB_OK;
}






















uint_8 g_cur_pitch = 0x01;

/**************************************************************************//*!
*
* @name  USB_Desc_Set_Cur_Pitch
*
* @brief  The function sets Current Pitch value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Sets Current Pitch value specified by the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Cur_Pitch(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (size);
    UNUSED (interface);
    /* set current pitch data*/
    g_cur_pitch = **data;
    return USB_OK;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Cur_Pitch
*
* @brief  The function gets Current Pitch value
*
* @param controller_ID   : Controller ID
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns Current Pitch value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Cur_Pitch(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [OUT] Pointer to Data */
USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (interface);
    /* get current pitch data*/
    *size=1;
    *data=&g_cur_pitch;
    return USB_OK;
}

uint_8 status_endpoint_data[5] = {0x01, 0x02, 0x03, 0x04, 0x05};
/**************************************************************************//*!
*
* @name  USB_Desc_Set_Mem_Endpoint
*
* @brief  The function gets endpoint memory value
*
* @param controller_ID   : Controller ID
* @param offset          : Offset
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns endpoint memory value to the Host
*****************************************************************************/
uint_8 USB_Desc_Set_Mem_Endpoint(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_16 offset,                     /* [IN] Offset */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    uint_16 index;

    UNUSED (controller_ID);
    UNUSED (interface);

    for(index = 0; index < *size ; index++)
    {   /* copy the report sent by the host */
        status_endpoint_data[offset + index] = *(*data + index);
    }
    *size = 0;

    return USB_OK;
}

/**************************************************************************//*!
*
* @name  USB_Desc_Get_Mem_Endpoint
*
* @brief  The function gets endpoint memory value
*
* @param controller_ID   : Controller ID
* @param offset          : Offset
* @param interface       : Interface
* @param data            : Pointer to Data to be send
* @param size            : Pointer to Size of Data
*
* @return :
*      USB_OK                  : When Successfull
*      USBERR_INVALID_REQ_TYPE : When  request for invalid
*                                Interface is presented
******************************************************************************
* Returns endpoint memory value to the Host
*****************************************************************************/
uint_8 USB_Desc_Get_Mem_Endpoint(
uint_8 controller_ID,               /* [IN] Controller ID */
uint_16 offset,                     /* [IN] Offset */
uint_8 interface,                   /* [IN] Interface */
uint_8_ptr *data,                   /* [IN] Pointer to Data */
USB_PACKET_SIZE *size               /* [IN] Pointer to Size of Data */
)
{
    UNUSED (controller_ID);
    UNUSED (interface);
    UNUSED (size);

    *data = &status_endpoint_data[offset];
    return USB_OK;
}


#if AUDIO_CLASS_2_0
/**************************************************************************//*!
 *
 * @name  USB_Desc_Get_Cur_Clock_Validity
 *
 * @brief  The function gets Current Sampling Frequency value
 *
 * @param controller_ID   : Controller ID
 * @param interface       : Interface
 * @param data            : Pointer to Data to be send
 * @param size            : Pointer to Size of Data
 *
 * @return :
 *      USB_OK                  : When Successfull
 *      USBERR_INVALID_REQ_TYPE : When  request for invalid
 *                                Interface is presented
 ******************************************************************************
 * Returns Current Clock state to the Host
 *****************************************************************************/
uint_8 USB_Desc_Get_Cur_Clock_Validity(
    uint_8 controller_ID,               /* [IN] Controller ID */
    uint_8 interface,                   /* [IN] Interface */
    uint_8_ptr *data,                   /* [OUT] Pointer to Data */
    USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
)
{
    static uint_8 val = TRUE;

    UNUSED (controller_ID);
    UNUSED (interface);

    *size = 1;
    *data = &val;
    return USB_OK;
}
#endif /* AUDIO_CLASS_2_0 */



/**************************************************************************//*!
*
* @name  USB_Desc_Get_Class_Architecture
*
* @brief The function returns with the list of all of class information ;ex: enpoint number
                                                            number interface, interface ID
*
* @param controller_ID : Controller ID
*
* @return pointer to DEV_ARCHITECTURE_STRUCT
*****************************************************************************
* This function returns the information about all the class information
* implemented
*****************************************************************************/
void* USB_Desc_Get_Class_Architecture(
uint_8 controller_ID      /* [IN] Controller ID */
)
{
    UNUSED (controller_ID)
    return (void*)&usb_dev_arc;
}
