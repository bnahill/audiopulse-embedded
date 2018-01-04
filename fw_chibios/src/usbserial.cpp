#include "usbserial.h"

void USBSerial::init(){
    sduObjectInit(&SDU);
    sduStart(&SDU, &serusbcfg);
    usbDisconnectBus(serusbcfg.usbp);
    chThdSleepMilliseconds(1500);
    usbStart(serusbcfg.usbp, &usbcfg);
    usbConnectBus(serusbcfg.usbp);
}

uint32_t USBSerial::read(uint8_t *dest, uint32_t len){
    uint32_t nbytes = chnRead(chSDU, dest, len);
    return nbytes;
}

uint32_t USBSerial::read_timeout(uint8_t *dest, uint32_t len, uint32_t timeout_ms){
    uint32_t nbytes = chnReadTimeout(chSDU, dest, len, MS2ST(timeout_ms));
    return nbytes;
}

void USBSerial::write(uint8_t const * data, uint32_t len){
    chnWrite(chSDU, data, len);
}

void USBSerial::sof_handler(USBDriver *usbp) {

  (void)usbp;

  osalSysLockFromISR();
  sduSOFHookI(&SDU);
  osalSysUnlockFromISR();
}


/*
 * Handles the USB driver global events.
 */
void USBSerial::usb_event(USBDriver *usbp, usbevent_t event) {
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
 * Handles the GET_DESCRIPTOR callback. All required descriptors must be
 * handled here.
 */
USBDescriptor const * USBSerial::get_descriptor(USBDriver *usbp,
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
