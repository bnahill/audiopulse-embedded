
#ifndef __APULSE_CTRL_IF_H_
#define __APULSE_CTRL_IF_H_

#include <hal.h>

class CtrlIF {
public:
    static void Initialize();

    static void cmd_debug_text(char const * s);

private:
    enum cmd_opcode : uint16_t {
        OP_SET_LED = 1,
        OP_SET_POWER = 2,
        OP_DBG_TEXT = 3
    };

    struct cmd_header {
        uint16_t len;
        enum cmd_opcode op;
    } __attribute__((packed));
    
    static constexpr uint32_t thread_stack_size = 4096;
    static constexpr uint32_t usb_write_timeout = MS2ST(500);
    static constexpr uint32_t usb_read_timeout = MS2ST(500);
    
    static THD_WORKING_AREA(thread_wa, thread_stack_size);
    static THD_FUNCTION(thread_fn, arg);
    
    static constexpr USBDriver &USBD = USBD1;
    
    static constexpr uint32_t USB_DATA_REQUEST_EP      = 1;
    static constexpr uint32_t USB_DATA_AVAILABLE_EP    = 1;
    static constexpr uint32_t USB_INTERRUPT_REQUEST_EP = 2;
    
    static SerialUSBDriver SDU;
    static constexpr BaseSequentialStream * bsSDU = reinterpret_cast<BaseSequentialStream *>(&SDU);
    static constexpr BaseAsynchronousChannel * chSDU = reinterpret_cast<BaseAsynchronousChannel *>(&SDU);
    
    static uint8_t const vcom_device_descriptor_data[18];
    static USBDescriptor const vcom_device_descriptor;
    static uint8_t const vcom_configuration_descriptor_data[67];
    static USBDescriptor const vcom_configuration_descriptor;
    static uint8_t const vcom_string0[4];
    static uint8_t const vcom_string1[38];
    static uint8_t const vcom_string2[56];
    static uint8_t const vcom_string3[8];
    static USBDescriptor const vcom_strings[4];
    
    static USBDescriptor const *get_descriptor(USBDriver *usbp,
                                               uint8_t dtype,
                                               uint8_t dindex,
                                               uint16_t lang);
    
    static USBInEndpointState ep1instate;

    static USBOutEndpointState ep1outstate;

    static const USBEndpointConfig ep1config;
    static USBInEndpointState ep2instate;
    static const USBEndpointConfig ep2config;
    static void usb_event(USBDriver *usbp, usbevent_t event);
    
    static void sof_handler(USBDriver *usbp);
    static const USBConfig usbcfg;
    static const SerialUSBConfig serusbcfg;
};

#endif // __APULSE_CTRL_IF_H_
