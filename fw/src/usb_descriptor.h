/******************************************************************************
 *
 * Freescale Semiconductor Inc.
 * (c) Copyright 2004-2009 Freescale Semiconductor, Inc.
 * Modifications (c) Copyright 2013 Ben Nahill
 * ALL RIGHTS RESERVED.
 *
 **************************************************************************//*!
 *
 * @file usb_descriptor.h
 *
 * @author
 *
 * @version
 *
 * @date May-28-2009
 *
 * @brief The file is a header file for USB Descriptors required for Keyboard
 *        Application
 *
 *****************************************************************************/

#ifndef _USB_DESCRIPTOR_H
#define _USB_DESCRIPTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "types.h"
#include "usb_class.h"

/******************************************************************************
 * Constants - None
 *****************************************************************************/

/******************************************************************************
 * Macro's
 *****************************************************************************/
#define REMOTE_WAKEUP_SHIFT              (5)
#define REMOTE_WAKEUP_SUPPORT            (TRUE)

/* Various descriptor sizes */
#define DEVICE_DESCRIPTOR_SIZE            (18)
//#define CONFIG_DESC_SIZE                  (34)
// Before switch to CDC
//#define CONFIG_DESC_SIZE                  (164 + 9)
#define CONFIG_DESC_SIZE                  (164)

#define DEVICE_QUALIFIER_DESCRIPTOR_SIZE  (10)
#define REPORT_DESC_SIZE                  (38)
#define CONFIG_ONLY_DESC_SIZE             (9)
#define IFACE_ONLY_DESC_SIZE              (9)
#define HID_ONLY_DESC_SIZE                (9)
#define ENDP_ONLY_DESC_SIZE               (7)
#define IAD_DESC_SIZE                     (0x08)
#define USB_IFACE_ASSOCIATION_DESCRIPTOR  (0x0B)
#define USB_AUDIO_DESCRIPTOR              (0x25)


/* HID buffer size */
#define HID_BUFFER_SIZE                     (64)
/* Max descriptors provided by the Application */
#define USB_MAX_STD_DESCRIPTORS             (8)
#define USB_MAX_CLASS_SPECIFIC_DESCRIPTORS  (2)
/* Max configuration supported by the Application */
#define USB_MAX_CONFIG_SUPPORTED            (1)

/* Max string descriptors supported by the Application */
#define USB_MAX_STRING_DESCRIPTORS        (4)

/* Max language codes supported by the USB */
#define USB_MAX_LANGUAGES_SUPPORTED       (1)


#define HID_DESC_ENDPOINT_COUNT         (2)
#define HID_ENDPOINT_IN                 (1)
#define HID_ENDPOINT_OUT                (2)
#define HID_ENDPOINT_PACKET_SIZE        (64)

#define DEVICE_QUALIFIER_DESCRIPTOR_SIZE  (10)
#define CONFIG_ONLY_DESC_SIZE             (9)
#define IFACE_ONLY_DESC_SIZE              (9)
#define AUDIO_ONLY_DESC_SIZE              (9)
#define AUDIO_ENDP_ONLY_DESC_SIZE               (9)
#define HEADER_ONLY_DESC_SIZE             (9)
#define INPUT_TERMINAL_ONLY_DESC_SIZE_A     (12)
#define OUTPUT_TERMINAL_ONLY_DESC_SIZE_A    (9)
#define FEATURE_UNIT_ONLY_DESC_SIZE       (13)
#define AUDIO_STREAMING_IFACE_DESC_SIZE   (7)
#define AUDIO_STREAMING_ENDP_DESC_SIZE    (7)
#define AUDIO_INTERFACE_DESC_TYPE_I_SIZE  (11)

#define USB_DEVICE_CLASS_AUDIO            (0x01)
#define USB_SUBCLASS_AUDIOCONTROL         (0x01)
#define USB_SUBCLASS_AUDIOSTREAM          (0x02)

#define AUDIO_INTERFACE_DESCRIPTOR_TYPE   (0x24)


#define AUDIO_CONTROL_HEADER                        (0x01)
#define AUDIO_CONTROL_INPUT_TERMINAL                (0x02)
#define AUDIO_CONTROL_OUTPUT_TERMINAL               (0x03)
#define AUDIO_CONTROL_MIXER_UNIT                    (0x04)
#define AUDIO_CONTROL_SELECTOR_UNIT                 (0x05)
#define AUDIO_CONTROL_FEATURE_UNIT                  (0x06)
#define AUDIO_CONTROL_EFFECT_UNIT                   (0x07)
#define AUDIO_CONTROL_PROCESSING_UNIT               (0x08)
#define AUDIO_CONTROL_EXTENSION_UNIT                (0x09)
#define AUDIO_CONTROL_CLOCK_SOURCE                  (0x0A)
#define AUDIO_CONTROL_CLOCK_SELECTOR                (0x0B)
#define AUDIO_CONTROL_CLOCK_MULTIPLIER              (0x0C)
#define AUDIO_CONTROL_SAMPLE_RATE_CONVERTER         (0x0D)

#define AUDIO_CONTROL_CLOCK_SOURCE_ID     (0x10)
#define AUDIO_CONTROL_INPUT_TERMINAL_ID   (0x20)
#define AUDIO_CONTROL_FEATURE_UNIT_ID     (0x30)

#define AUDIO_CONTROL_OUTPUT_TERMINAL     (0x03)
#define AUDIO_STREAMING_GENERAL           (0x01)
#define AUDIO_STREAMING_FORMAT_TYPE       (0x02)
#define AUDIO_FORMAT_TYPE_I               (0x01)
#define AUDIO_ENDPOINT_GENERAL            (0x01)

/* Audio controls at Feature Unit descriptor level */
#define AUDIO_MUTE_CONTROL                (0x01)
#define AUDIO_VOLUME_CONTROL              (0x02)
#define AUDIO_BASS_CONTROL                (0x04)
#define AUDIO_MID_CONTROL                 (0x08)
#define AUDIO_TREBLE_CONTROL              (0x10)
#define AUDIO_GRAPHIC_EQ_CONTROL          (0x20)
#define AUDIO_AUTOMATIC_GAIN_CONTROL      (0x40)
#define AUDIO_DELAY_CONTROL               (0x80)

#define AUDIO_DESC_ENDPOINT_COUNT       (1)
#define AUDIO_UNIT_COUNT                (3)
#define AUDIO_ENDPOINT_PACKET_SIZE      (256)
#define AUDIO_ENDPOINT_SAMPLE_SIZE      (4)
#define AUDIO_ENDPOINT_PACKET_PERIOD    (1 + 1)

#define SPEAKER_CTRL_INTF               (2)

#define COMP_CLASS_UNIT_COUNT           (2)
#define COMPOSITE_DESC_ENDPOINT_COUNT   (3)

#define DIRECT_LINE_CONTROL_MODEL           (0x01)
#define ABSTRACT_CONTROL_MODEL              (0x02)
#define TELEPHONE_CONTROL_MODEL             (0x03)
#define MULTI_CHANNEL_CONTROL_MODEL         (0x04)
#define CAPI_CONTROL_MOPDEL                 (0x05)
#define ETHERNET_NETWORKING_CONTROL_MODEL   (0x06)
#define ATM_NETWORKING_CONTROL_MODEL        (0x07)
#define WIRELESS_HANDSET_CONTROL_MODEL      (0x08)
#define DEVICE_MANAGEMENT                   (0x09)
#define MOBILE_DIRECT_LINE_MODEL            (0x0A)
#define OBEX                                (0x0B)
#define ETHERNET_EMULATION_MODEL            (0x0C)


#define CDC_DESC_ENDPOINT_COUNT       (2)
#define DIC_BULK_IN_ENDPOINT          (1)
#define DIC_BULK_OUT_ENDPOINT         (2)
#define DIC_BULK_IN_ENDP_PACKET_SIZE  (64)
#define DIC_BULK_OUT_ENDP_PACKET_SIZE (DIC_BULK_IN_ENDP_PACKET_SIZE)

/* Communication Class Protocol Codes */
#define NO_CLASS_SPECIFIC_PROTOCOL  (0x00)
#define AT_250_PROTOCOL             (0x01)
#define AT_PCCA_101_PROTOCOL        (0x02)
#define AT_PCCA_101_ANNEX_O         (0x03)
#define AT_GSM_7_07                 (0x04)
#define AT_3GPP_27_007              (0x05)
#define AT_TIA_CDMA                 (0x06)
#define ETHERNET_EMULATION_PROTOCOL (0x07)
#define EXTERNAL_PROTOCOL           (0xFE)
#define VENDOR_SPECIFIC             (0xFF)

/* Data Class Protocol Codes */
/* #define NO_CLASS_SPECIFIC_PROTOCOL  (0x00) */
#define PYHSICAL_INTERFACE_PROTOCOL    (0x30)
#define HDLC_PROTOCOL                  (0x31)
#define TRANSPARENT_PROTOCOL           (0x32)
#define MANAGEMENT_PROTOCOL            (0x50)
#define DATA_LINK_Q931_PROTOCOL        (0x51)
#define DATA_LINK_Q921_PROTOCOL        (0x52)
#define DATA_COMPRESSION_V42BIS        (0x90)
#define EURO_ISDN_PROTOCOL             (0x91)
#define RATE_ADAPTION_ISDN_V24         (0x92)
#define CAPI_COMMANDS                  (0x93)
#define HOST_BASED_DRIVER              (0xFD)
#define CDC_UNIT_FUNCTIONAL            (0xFE)

#define LINE_CODING_SIZE              (0x07)
#define COMM_FEATURE_DATA_SIZE        (0x02)

#define LINE_CODE_DTERATE_IFACE0      (115200) /*e.g 9600 is 0x00002580 */
#define LINE_CODE_CHARFORMAT_IFACE0   (0x00)   /* 1 stop bit */
#define LINE_CODE_PARITYTYPE_IFACE0   (0x00)   /* No Parity */
#define LINE_CODE_DATABITS_IFACE0     (0x08)   /* Data Bits Format */

#define LINE_CODE_DTERATE_IFACE1      (9600)   /*e.g. 115200 is 0x0001C200*/
#define LINE_CODE_CHARFORMAT_IFACE1   (0x00)   /* 1 stop bit */
#define LINE_CODE_PARITYTYPE_IFACE1   (0x00)   /* No Parity */
#define LINE_CODE_DATABITS_IFACE1     (0x08)   /* Data Bits Format */


/* string descriptors sizes */
#define USB_STR_DESC_SIZE               (2)
#define USB_STR_0_SIZE                  (2)
#define USB_STR_1_SIZE                  (38)
#define USB_STR_2_SIZE                  (34)
#define USB_STR_3_SIZE                  (6)
#define USB_STR_n_SIZE                  (32)


/* descriptors codes */
#define USB_DEVICE_DESCRIPTOR     (1)
#define USB_CONFIG_DESCRIPTOR     (2)
#define USB_STRING_DESCRIPTOR     (3)
#define USB_IFACE_DESCRIPTOR      (4)
#define USB_ENDPOINT_DESCRIPTOR   (5)
#define USB_DEVQUAL_DESCRIPTOR    (6)
#define USB_HID_DESCRIPTOR        (0x21)
#define USB_REPORT_DESCRIPTOR     (0x22)

#define USB_MAX_SUPPORTED_INTERFACES     (3)


#define HID_ENDPOINT_IN                    (1)
#define HID_ENDPOINT_OUT                   (2)
#define AUDIO_ENDPOINT                     (3)

#define HID_ENDPOINT_SIZE                  (64)
#define AUDIO_ENDPOINT_SIZE                (AUDIO_ENDPOINT_PACKET_SIZE)

/******************************************************************************
 * Types
 *****************************************************************************/
typedef const struct _USB_LANGUAGE
{
    uint_16 const language_id;      /* Language ID */
    uint_8 const ** lang_desc;      /* Language Descriptor String */
    uint_8 const * lang_desc_size;  /* Language Descriptor Size */
} USB_LANGUAGE;

typedef const struct _USB_ALL_LANGUAGES
{
    /* Pointer to Supported Language String */
    uint_8 const *languages_supported_string;
    /* Size of Supported Language String */
    uint_8 const languages_supported_size;
    /* Array of Supported Languages */
    USB_LANGUAGE usb_language[USB_MAX_SUPPORTED_INTERFACES];
}USB_ALL_LANGUAGES;

typedef struct _USB_ENDPOINTS
{
    /* Number of non control Endpoints */
    uint_8 count;
    /* Array of Endpoints Structures */
    USB_EP_STRUCT ep[COMPOSITE_DESC_ENDPOINT_COUNT];
}USB_ENDPOINTS,_PTR_ USB_ENDPOINTS_PTR;


typedef struct _DEV_ARCHITECTURE_STRUCT
{
    /* Class count */
    uint_8 cl_count;
    /* Endpoint count */
    uint_8 ep_count;
    /* Sub-class info */
    uint_8* value[COMP_CLASS_UNIT_COUNT];
}DEV_ARCHITECTURE_STRUCT, _PTR_ DEV_ARCHITECTURE_STRUCT_PTR;

typedef struct _USB_AUDIO_UT_STRUCT
{
    uint_8          unit_id;      /* endpoint number         */
    uint_8          type;        /* type of endpoint        */
}USB_UT_STRUCT, *USB_UT_STRUCT_PTR;

typedef const struct _USB_AUDIO_UNITS
{
    /* Number of entities of this application */
    uint_8         count;
    /* Array of entities of this application */
    USB_UT_STRUCT  et[AUDIO_UNIT_COUNT];
}USB_AUDIO_UNITS;


/******************************************************************************
 * Global Functions
 *****************************************************************************/
extern boolean USB_Desc_Remote_Wakeup(uint_8 controller_ID);

extern void* USB_Desc_Get_Endpoints(uint_8 controller_ID);
extern uint_8 USB_Desc_Get_Descriptor(
                                uint_8 controller_ID,
                                uint_8 type,
                                uint_8 str_num,
                                uint_16 index,
                                uint_8_ptr *descriptor,
                                USB_PACKET_SIZE *size);

extern uint_8 USB_Desc_Get_Interface(
                                uint_8 controller_ID,
                                uint_8 interface,
                                uint_8_ptr alt_interface);


extern uint_8 USB_Desc_Set_Interface(
                                uint_8 controller_ID,
                                uint_8 interface,
                                uint_8 alt_interface);

extern boolean USB_Desc_Valid_Configation(
                                uint_8 controller_ID,
                                uint_16 config_val);

extern boolean USB_Desc_Valid_Interface(
                                uint_8 controller_ID,
                                uint_8 interface);



extern uint_8 USB_Desc_Set_Copy_Protect(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Copy_Protect(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Mute(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Volume(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Min_Volume(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Max_Volume(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Res_Volume(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Mute(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Max_Volume(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Min_Volume(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Volume(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Res_Volume(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Min_Bass(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Min_Bass(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Max_Bass(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Max_Bass(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Bass(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Bass(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Res_Bass(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Res_Bass(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Mid(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Min_Mid(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Max_Mid(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Res_Mid(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Mid(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Min_Mid(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Max_Mid(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Res_Mid(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Treble(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Res_Treble(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Min_Treble(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Max_Treble(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Treble(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Res_Treble(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Min_Treble(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Max_Treble(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Res_Graphic_Equalizer(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Graphic_Equalizer(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Min_Graphic_Equalizer(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Max_Graphic_Equalizer(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Res_Graphic_Equalizer(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Graphic_Equalizer(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Min_Graphic_Equalizer(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Max_Graphic_Equalizer(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Automatic_Gain(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Automatic_Gain(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Delay(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Min_Delay(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Max_Delay(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Res_Delay(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Delay(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Min_Delay(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Max_Delay(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Res_Delay(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Bass_Boost(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Bass_Boost(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Loudness(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Loudness(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Sampling_Frequency(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Min_Sampling_Frequency(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Max_Sampling_Frequency(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Res_Sampling_Frequency(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Sampling_Frequency(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Range_Sampling_Frequency(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Min_Sampling_Frequency(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Max_Sampling_Frequency(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Res_Sampling_Frequency(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Cur_Pitch(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Pitch(
uint_8 controller_ID,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Set_Mem_Endpoint(
uint_8 controller_ID,
uint_16 offset,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Mem_Endpoint(
uint_8 controller_ID,
uint_16 offset,
uint_8 interface,
uint_8_ptr *data,
USB_PACKET_SIZE *size
);
extern uint_8 USB_Desc_Get_Cur_Clock_Validity(
    uint_8 controller_ID,               /* [IN] Controller ID */
    uint_8 interface,                   /* [IN] Interface */
    uint_8_ptr *data,                   /* [OUT] Pointer to Data */
    USB_PACKET_SIZE *size               /* [OUT] Pointer to Size of Data */
);

#ifdef __cplusplus
}
#endif

#endif
