# -*- coding: utf-8 -*-

import struct
import usb.core
import usb.util
import sys
import numpy as np


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
    GET_REPORT = 0x01
    GET_IDLE = 0x02
    GET_PROTOCOL = 0x03
    SET_REPORT = 0x09
    SET_IDLE = 0x0A
    SET_PROTOCOL = 0x0B


class HidReportConstants:
    INPUT = 0x01
    OUTPUT = 0x02
    FEATURE = 0x03


class Constants():
    CMD_RESET = 0
    CMD_STARTUP = 1
    CMD_SETUPTONES = 2
    CMD_SETUPCAPTURE = 3
    CMD_GETSTATUS = 4
    CMD_GETDATA = 5
    CMD_START = 6
    CMD_CALIBRATE_MIC = 7
    CMD_RESET_CALIB = 8

    TONE_OFF = 0
    TONE_FIXED = 1
    TONE_CHIRP = 2


class APulseStatus(object):
    WG_RESET = 0
    WG_READY = 1
    WG_RUNNING = 2
    WG_DONE = 3

    str_wgstate = ["Reset", "Ready", "Running", "Done"]

    IN_RESET = 0
    IN_READY = 1
    IN_RUNWAIT = 2
    IN_CAPTURING = 3
    IN_DONE = 4

    str_instate = ["Reset", "Ready", "RunWait", "Capturing", "Done"]

    TEST_RESET = 0
    TEST_CONFIGURING = 1
    TEST_READY = 2
    TEST_RUNNING = 3
    TEST_DONE = 4
    TEST_CALIB_MIC = 5

    str_test = ["Reset", "Configuring", "Ready",
                "Running", "Done", "Calibrating"]

    def __init__(self, data):
        assert len(data) == 5, "Not a status packet... Len:%d" % len(data)
        (self.version, self.input_state, self.wavegen_state,
         self.controller_state, self.err_code) = struct.unpack("B" * 5, data)

    def __str__(self):
        input_state = self.str_instate[self.input_state]
        wavegen_state = self.str_wgstate[self.wavegen_state]
        control_state = self.str_test[self.controller_state]
        s = "Version:{}, Input:{}, WaveGen:{}, Control:{}, Err:{}".format(
            self.version, input_state, wavegen_state,
            control_state, self.err_code
        )
        return s

    def is_done(self):
        return self.controller_state == self.TEST_DONE


class ToneConfig(object):
    TONE_OFF = 0
    TONE_FIXED = 1
    TONE_CHIRP = 2

    def __init__(self, typ, f1, f2, t1, t2, db, ch):
        self.typ = typ
        self.f1 = f1
        self.f2 = f2
        self.t1 = t1
        self.t2 = t2
        self.db = db
        self.ch = ch

    def to_buff(self):
        return struct.pack("<BHHHHH",
            (self.ch << 4) | self.typ,
            int(self.db) << 8, self.f1, self.f2, self.t1, self.t2)


class FixedTone(ToneConfig):
    def __init__(self, f1, t1, t2, db, ch):
        super(FixedTone, self).__init__(ToneConfig.TONE_FIXED, f1, 0,
                                        t1, t2, db, ch)


class DummyTone(ToneConfig):
    def __init__(self):
        super(DummyTone, self).__init__(ToneConfig.TONE_OFF, 0, 0, 0, 0, 0, 0)


class APulseIface(object):
    dev = None
    out_ep = None
    in_ep = None

    def __init__(self):
        pass

    def disconnect(self):
        if self.dev:
            self.dev.reset()

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
        intf = cfg[(0, 0)]

        ep = usb.util.find_descriptor(
            intf,
            # match the first IN endpoint (no OUT currently...)
            custom_match=lambda e:
                usb.util.endpoint_direction(e.bEndpointAddress) ==
                usb.util.ENDPOINT_IN
        )

        assert ep is not None

        self.in_ep = ep
        self.out_ep = ep
        self.dev = dev

    def reset(self):
        data = struct.pack("B", Constants.CMD_RESET)
        self._write(data)

    def get_status(self):
        data = self._read(5)
        return APulseStatus(data)

    def config_tones(self, tones):
        buff = struct.pack("B", Constants.CMD_SETUPTONES)
        for t in tones:
            buff = buff + t.to_buff()
        for i in range(3 - len(tones)):
            buff = buff + DummyTone().to_buff()
        assert len(buff) == 34, "Wrong sized buffer..."
        self._write(buff)

    def config_capture(self, t1, t2, overlap):
        assert t2 > t1, "Non-positive record time!"
        epochs = int(np.ceil((t2 - t1) * (16.0 / (512 - overlap)) - 1))
        buff = struct.pack("<BHBHH", Constants.CMD_SETUPCAPTURE,
            overlap, 0, epochs, t1)
        self._write(buff)

    def start(self):
        buff = struct.pack("B", Constants.CMD_START)
        self._write(buff)
        status = self.get_status()
        if(status.err_code):
            sys.stderr.write("Error starting! {}".format(status.err_code))

    def calibrate(self):
        self._write(struct.pack("B", Constants.CMD_CALIBRATE_MIC))

    def decalibrate(self):
        self._write(struct.pack("B", Constants.CMD_RESET_CALIB))

    def get_data(self):
        psd = np.zeros(257, dtype=np.float128)
        avg = np.zeros(512, dtype=np.float128)

        if not self.get_status().is_done():
            sys.stderr.write("Could not get data; test incomplete\n")
            return (psd, avg)

        self._write(struct.pack("B", Constants.CMD_GETDATA))
        for i in range(16):
            data = self._read(64)
            psd[i * 16:(i + 1) * 16] = struct.unpack("<" + "i" * 16, data)
        (psd[256],) = struct.unpack("<i", self._read(4))

        for i in range(32):
            data = self._read(64)
            avg[i * 16:(i + 1) * 16] = struct.unpack("<" + "i" * 16, data)

        # Normalize around 90dB
        #psd /= np.float128(0x7FFFFFFF)
        #avg /= np.float128(0x7FFFFFFF)

        return (psd, avg)

    def _write(self, data):
        self.dev.ctrl_transfer(ReqConstants.DIR_OUT |
                               ReqConstants.RECIPIENT_DEVICE |
                               ReqConstants.TYPE_CLASS,
                               HidConstants.SET_REPORT,
                               (HidReportConstants.OUTPUT << 8) | 0x00,
                               0,
                               data)

    def _read(self, n):
        return self.dev.ctrl_transfer(ReqConstants.DIR_IN |
                                      ReqConstants.RECIPIENT_INTERFACE |
                                      ReqConstants.TYPE_CLASS,
                                      HidConstants.GET_REPORT,
                                      (HidReportConstants.INPUT << 8) | 0x00,
                                      0,
                                      n)



