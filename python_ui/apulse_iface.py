# -*- coding: utf-8 -*-

import struct
import usb.core
import usb.util
import sys

class UsbConstants():
    USB_VID = 0x15A2
    USB_PID = 0xBEEF

class ReqConstants():
    DIR_IN = 0x80
    DIR_OUT = 0x00

    TYPE_STANDARD = (0x00 << 5)
    # Class
    TYPE_CLASS = (0x01 << 5)
    # Vendor
    TYPE_VENDOR = (0x02 << 5)
    # Reserved
    TYPE_RESERVED = (0x03 << 5)

    RECIPIENT_DEVICE = 0x00
    # Interface
    RECIPIENT_INTERFACE = 0x01
    # Endpoint
    RECIPIENT_ENDPOINT = 0x02
    # Other
    RECIPIENT_OTHER = 0x03

class HidConstants:
    GET_REPORT           =     0x01
    GET_IDLE             =     0x02
    GET_PROTOCOL         =     0x03
    SET_REPORT           =     0x09
    SET_IDLE             =     0x0A
    SET_PROTOCOL         =     0x0B

class HidReportConstants:
    INPUT    =     0x01
    OUTPUT   =     0x02
    FEATURE  =     0x03


class Constants():
    CMD_RESET        = 0
    CMD_STARTUP      = 1
    CMD_SETUPTONES   = 2
    CMD_SETUPCAPTURE = 3
    CMD_GETSTATUS    = 4
    CMD_GETDATA      = 5
    CMD_START        = 6

    TONE_OFF         = 0
    TONE_FIXED       = 1
    TONE_CHIRP       = 2

class APulseIface():
    dev = None
    out_ep = None
    in_ep = None

    def __init__(self):
        pass

    def connect(self):
        dev = usb.core.find(idVendor=UsbConstants.USB_VID,
                            idProduct=UsbConstants.USB_PID)
        if dev is None:
            raise ValueError("No device found")


        if dev.is_kernel_driver_active(0) is True:
            sys.stderr.write("Releasing kernel driver\n")
            dev.detach_kernel_driver(0)

        dev.set_configuration()

        cfg = dev.get_active_configuration()
        intf = cfg[(0,0)]

        ep = usb.util.find_descriptor(
            intf,
            # match the first IN endpoint (no OUT currently...)
            custom_match = \
            lambda e: \
                usb.util.endpoint_direction(e.bEndpointAddress) == \
                usb.util.ENDPOINT_IN
        )

        assert ep is not None

        self.in_ep = ep
        self.out_ep = ep
        self.dev = dev

    def _write(self, data):
        self.dev.ctrl_transfer(ReqConstants.DIR_OUT |
                               ReqConstants.RECIPIENT_DEVICE |
                               ReqConstants.TYPE_CLASS,
                               HidConstants.SET_REPORT,
                               (HidReportConstants.OUTPUT<<8) | 0x00, 0, data)

    def _read(self, n):
        return self.dev.ctrl_transfer(ReqConstants.DIR_IN |
                                      ReqConstants.RECIPIENT_INTERFACE |
                                      ReqConstants.TYPE_CLASS,
                                      HidConstants.GET_REPORT,
                                      (HidReportConstants.INPUT<<8) | 0x00, 0, 0)

    def reset(self):
        data = struct.pack("b", Constants.CMD_RESET)
        self._write(data)


