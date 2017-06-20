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

typedef struct {
    uint8_t led;
    uint8_t value;
} __attribute__((packed)) payload_set_led_t;

typedef struct {
    uint8_t value;
} __attribute__((packed)) payload_set_power_t;

THD_WORKING_AREA(CtrlIF::thread_wa, thread_stack_size);

static AK4621 codec(*SAI2, 1, *SAI1, 0, SPID3, LINE_SPI1_CS_CODEC, LINE_CODEC_PDN, LINE_CLK_EN,
                    STM32_SAI1A_DMA_CHN, STM32_SAI_SAI1A_DMA_STREAM, STM32_SAI_SAI2B_DMA_PRIORITY, STM32_SAI_SAI1A_DMA_IRQ_PRIORITY,
                    STM32_SAI2B_DMA_CHN, STM32_SAI_SAI2B_DMA_STREAM, STM32_SAI_SAI1A_DMA_PRIORITY, STM32_SAI_SAI2B_DMA_IRQ_PRIORITY);

THD_FUNCTION(CtrlIF::thread_fn, arg){
    (void)arg;
    chRegSetThreadName("CtrlIF");
    static uint8_t cmd_buffer[1024] = {0};
    uint16_t cmd_len;
    uint16_t cmd_op;
    uint16_t nbytes;


    codec.init_hw();
    codec.init();


    while (true) {
        /*
        palSetLine(LINE_LED1);
        chThdSleepMilliseconds(100);
        palClearLine(LINE_LED1);
        chThdSleepMilliseconds(100);
        palSetLine(LINE_LED1);
        chThdSleepMilliseconds(100);
        palClearLine(LINE_LED1);
        chThdSleepMilliseconds(1000);
        */
        chThdSleepMilliseconds(10);
        nbytes = chnReadTimeout(chSDU, reinterpret_cast<uint8_t *>(&cmd_len), 2, usb_read_timeout);
        if((nbytes != 2) || (cmd_len > sizeof(cmd_buffer)))
            continue;
        nbytes = chnReadTimeout(chSDU, reinterpret_cast<uint8_t *>(&cmd_op), 2, usb_read_timeout);
        if(nbytes != 2)
            continue;
        nbytes = chnReadTimeout(chSDU, cmd_buffer, cmd_len, usb_read_timeout);
        if(nbytes != cmd_len)
            continue;
        
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
                    palWriteLine(LINE_ANALOG_EN, (payload->value) ? PAL_HIGH : PAL_LOW);
                }
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

    chnWrite(chSDU, reinterpret_cast<uint8_t const *>(&header), sizeof(header));
    chnWrite(chSDU, reinterpret_cast<uint8_t const *>(s), header.len);
}

void CtrlIF::Initialize(){
    sduObjectInit(&SDU);
    sduStart(&SDU, &serusbcfg);
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1500);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);
    
    chThdCreateStatic(thread_wa, sizeof(thread_wa), LOWPRIO, thread_fn, NULL);
}

/* Virtual serial port over USB.*/
SerialUSBDriver CtrlIF::SDU;

/*
 * USB Device Descriptor.
 */
uint8_t const CtrlIF::vcom_device_descriptor_data[18] = {
  USB_DESC_DEVICE       (0x0200,        /* bcdUSB (2.0).                    */
                         0x02,          /* bDeviceClass (CDC).              */
                         0x00,          /* bDeviceSubClass.                 */
                         0x00,          /* bDeviceProtocol.                 */
                         0x40,          /* bMaxPacketSize.                  */
                         0x0483,        /* idVendor (ST).                   */
                         0x5740,        /* idProduct.                       */
                         0x0200,        /* bcdDevice.                       */
                         1,             /* iManufacturer.                   */
                         2,             /* iProduct.                        */
                         3,             /* iSerialNumber.                   */
                         1)             /* bNumConfigurations.              */
};

/*
 * Device Descriptor wrapper.
 */
USBDescriptor const CtrlIF::vcom_device_descriptor = {
  sizeof(vcom_device_descriptor_data),
  vcom_device_descriptor_data
};

/* Configuration Descriptor tree for a CDC.*/
uint8_t const CtrlIF::vcom_configuration_descriptor_data[67] = {
  /* Configuration Descriptor.*/
  USB_DESC_CONFIGURATION(67,            /* wTotalLength.                    */
                         0x02,          /* bNumInterfaces.                  */
                         0x01,          /* bConfigurationValue.             */
                         0,             /* iConfiguration.                  */
                         0xC0,          /* bmAttributes (self powered).     */
                         50),           /* bMaxPower (100mA).               */
  /* Interface Descriptor.*/
  USB_DESC_INTERFACE    (0x00,          /* bInterfaceNumber.                */
                         0x00,          /* bAlternateSetting.               */
                         0x01,          /* bNumEndpoints.                   */
                         0x02,          /* bInterfaceClass (Communications
                                           Interface Class, CDC section
                                           4.2).                            */
                         0x02,          /* bInterfaceSubClass (Abstract
                                         Control Model, CDC section 4.3).   */
                         0x01,          /* bInterfaceProtocol (AT commands,
                                           CDC section 4.4).                */
                         0),            /* iInterface.                      */
  /* Header Functional Descriptor (CDC section 5.2.3).*/
  USB_DESC_BYTE         (5),            /* bLength.                         */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x00),         /* bDescriptorSubtype (Header
                                           Functional Descriptor.           */
  USB_DESC_BCD          (0x0110),       /* bcdCDC.                          */
  /* Call Management Functional Descriptor. */
  USB_DESC_BYTE         (5),            /* bFunctionLength.                 */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x01),         /* bDescriptorSubtype (Call Management
                                           Functional Descriptor).          */
  USB_DESC_BYTE         (0x00),         /* bmCapabilities (D0+D1).          */
  USB_DESC_BYTE         (0x01),         /* bDataInterface.                  */
  /* ACM Functional Descriptor.*/
  USB_DESC_BYTE         (4),            /* bFunctionLength.                 */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x02),         /* bDescriptorSubtype (Abstract
                                           Control Management Descriptor).  */
  USB_DESC_BYTE         (0x02),         /* bmCapabilities.                  */
  /* Union Functional Descriptor.*/
  USB_DESC_BYTE         (5),            /* bFunctionLength.                 */
  USB_DESC_BYTE         (0x24),         /* bDescriptorType (CS_INTERFACE).  */
  USB_DESC_BYTE         (0x06),         /* bDescriptorSubtype (Union
                                           Functional Descriptor).          */
  USB_DESC_BYTE         (0x00),         /* bMasterInterface (Communication
                                           Class Interface).                */
  USB_DESC_BYTE         (0x01),         /* bSlaveInterface0 (Data Class
                                           Interface).                      */
  /* Endpoint 2 Descriptor.*/
  USB_DESC_ENDPOINT     (USB_INTERRUPT_REQUEST_EP|0x80,
                         0x03,          /* bmAttributes (Interrupt).        */
                         0x0008,        /* wMaxPacketSize.                  */
                         0xFF),         /* bInterval.                       */
  /* Interface Descriptor.*/
  USB_DESC_INTERFACE    (0x01,          /* bInterfaceNumber.                */
                         0x00,          /* bAlternateSetting.               */
                         0x02,          /* bNumEndpoints.                   */
                         0x0A,          /* bInterfaceClass (Data Class
                                           Interface, CDC section 4.5).     */
                         0x00,          /* bInterfaceSubClass (CDC section
                                           4.6).                            */
                         0x00,          /* bInterfaceProtocol (CDC section
                                           4.7).                            */
                         0x00),         /* iInterface.                      */
  /* Endpoint 3 Descriptor.*/
  USB_DESC_ENDPOINT     (USB_DATA_AVAILABLE_EP,       /* bEndpointAddress.*/
                         0x02,          /* bmAttributes (Bulk).             */
                         0x0040,        /* wMaxPacketSize.                  */
                         0x00),         /* bInterval.                       */
  /* Endpoint 1 Descriptor.*/
  USB_DESC_ENDPOINT     (USB_DATA_REQUEST_EP|0x80,    /* bEndpointAddress.*/
                         0x02,          /* bmAttributes (Bulk).             */
                         0x0040,        /* wMaxPacketSize.                  */
                         0x00)          /* bInterval.                       */
};

/*
 * Configuration Descriptor wrapper.
 */
USBDescriptor const CtrlIF::vcom_configuration_descriptor = {
  sizeof vcom_configuration_descriptor_data,
  vcom_configuration_descriptor_data
};

/*
 * U.S. English language identifier.
 */
uint8_t const CtrlIF::vcom_string0[] = {
  USB_DESC_BYTE(4),                     /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  USB_DESC_WORD(0x0409)                 /* wLANGID (U.S. English).          */
};

/*
 * Vendor string.
 */
uint8_t const CtrlIF::vcom_string1[] = {
  USB_DESC_BYTE(38),                    /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
  'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
  'c', 0, 's', 0
};

/*
 * Device Description string.
 */
uint8_t const CtrlIF::vcom_string2[] = {
  USB_DESC_BYTE(56),                    /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  'C', 0, 'h', 0, 'i', 0, 'b', 0, 'i', 0, 'O', 0, 'S', 0, '/', 0,
  'R', 0, 'T', 0, ' ', 0, 'V', 0, 'i', 0, 'r', 0, 't', 0, 'u', 0,
  'a', 0, 'l', 0, ' ', 0, 'C', 0, 'O', 0, 'M', 0, ' ', 0, 'P', 0,
  'o', 0, 'r', 0, 't', 0
};

/*
 * Serial Number string.
 */
uint8_t const CtrlIF::vcom_string3[] = {
  USB_DESC_BYTE(8),                     /* bLength.                         */
  USB_DESC_BYTE(USB_DESCRIPTOR_STRING), /* bDescriptorType.                 */
  '0' + CH_KERNEL_MAJOR, 0,
  '0' + CH_KERNEL_MINOR, 0,
  '0' + CH_KERNEL_PATCH, 0
};

/*
 * Strings wrappers array.
 */
USBDescriptor const CtrlIF::vcom_strings[] = {
  {sizeof vcom_string0, vcom_string0},
  {sizeof vcom_string1, vcom_string1},
  {sizeof vcom_string2, vcom_string2},
  {sizeof vcom_string3, vcom_string3}
};

/*
 * Handles the GET_DESCRIPTOR callback. All required descriptors must be
 * handled here.
 */
USBDescriptor const * CtrlIF::get_descriptor(USBDriver *usbp,
                                           uint8_t dtype,
                                           uint8_t dindex,
                                           uint16_t lang) {

  (void)usbp;
  (void)lang;
  switch (dtype) {
  case USB_DESCRIPTOR_DEVICE:
    return &vcom_device_descriptor;
  case USB_DESCRIPTOR_CONFIGURATION:
    return &vcom_configuration_descriptor;
  case USB_DESCRIPTOR_STRING:
    if (dindex < 4)
      return &vcom_strings[dindex];
  }
  return NULL;
}

/**
 * @brief   IN EP1 state.
 */
USBInEndpointState CtrlIF::ep1instate;

/**
 * @brief   OUT EP1 state.
 */
USBOutEndpointState CtrlIF::ep1outstate;

/**
 * @brief   EP1 initialization structure (both IN and OUT).
 */
const USBEndpointConfig CtrlIF::ep1config = {
    USB_EP_MODE_TYPE_BULK,
    NULL,
    sduDataTransmitted,
    sduDataReceived,
    0x0040,
    0x0040,
    &ep1instate,
    &ep1outstate,
    4,
    NULL
};

/**
 * @brief   IN EP2 state.
 */
USBInEndpointState CtrlIF::ep2instate;

/**
 * @brief   EP2 initialization structure (IN only).
 */
USBEndpointConfig const CtrlIF::ep2config = {
    USB_EP_MODE_TYPE_INTR,
    NULL,
    sduInterruptTransmitted,
    NULL,
    0x0010,
    0x0000,
    &ep2instate,
    NULL,
    1,
    NULL
};

/*
 * Handles the USB driver global events.
 */
void CtrlIF::usb_event(USBDriver *usbp, usbevent_t event) {
  switch (event) {
  case USB_EVENT_ADDRESS:
    return;
  case USB_EVENT_CONFIGURED:
    chSysLockFromISR();

    /* Enables the endpoints specified into the configuration.
       Note, this callback is invoked from an ISR so I-Class functions
       must be used.*/
    usbInitEndpointI(usbp, USB_DATA_REQUEST_EP, &ep1config);
    usbInitEndpointI(usbp, USB_INTERRUPT_REQUEST_EP, &ep2config);

    /* Resetting the state of the CDC subsystem.*/
    sduConfigureHookI(&SDU);

    chSysUnlockFromISR();
    return;
  case USB_EVENT_RESET:
    /* Falls into.*/
  case USB_EVENT_UNCONFIGURED:
    /* Falls into.*/
  case USB_EVENT_SUSPEND:
    chSysLockFromISR();

    /* Disconnection event on suspend.*/
    sduSuspendHookI(&SDU);

    chSysUnlockFromISR();
    return;
  case USB_EVENT_WAKEUP:
    chSysLockFromISR();

    /* Disconnection event on suspend.*/
    sduWakeupHookI(&SDU);

    chSysUnlockFromISR();
    return;
  case USB_EVENT_STALLED:
    return;
  }
  return;
}

/*
 * Handles the USB driver global events.
 */
void CtrlIF::sof_handler(USBDriver *usbp) {

  (void)usbp;

  osalSysLockFromISR();
  sduSOFHookI(&SDU);
  osalSysUnlockFromISR();
}

/*
 * USB driver configuration.
 */
const USBConfig CtrlIF::usbcfg = {
  usb_event,
  get_descriptor,
  sduRequestsHook,
  sof_handler
};

/*
 * Serial over USB driver configuration.
 */
const SerialUSBConfig CtrlIF::serusbcfg = {
  &USBD,
  USB_DATA_REQUEST_EP,
  USB_DATA_AVAILABLE_EP,
  USB_INTERRUPT_REQUEST_EP
};

