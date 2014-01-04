package com.apulse.usbtest;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;

import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.hardware.usb.UsbInterface;
import android.hardware.usb.UsbEndpoint;
import android.hardware.usb.UsbDeviceConnection;
import android.util.Log;

import java.util.HashMap;

/**
 * Created by ben on 12/25/13.
 */
public class USBIface {
    protected Context context;
    protected Activity activity;
    protected USBConnHandler handler;
    protected UsbManager manager;

    protected PendingIntent mPermissionIntent;

    public static final int max_transfer_size = 64;

    public static final int usb_vid = 0x15A2;
    public static final int usb_pid = 0xBEEF;

    static public abstract class USBConnHandler {
        public abstract void handleConnected();
        public abstract void handleError();
    };

    public USBIface(Activity _activity){
        // Nothing for now...
        device = null;
        activity = _activity;
        context = activity.getBaseContext();
        error = "";
        manager = (UsbManager)context.getSystemService(Context.USB_SERVICE);
    }

    /*
     * @brief BroadcastReceiver for requesting permission for a device
     *
     */
    private class UsbReceiver extends BroadcastReceiver {
        @Override
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d("musbreceiver", "received broadcast");
            if (ACTION_USB_PERMISSION.equals(action)) {
                synchronized (this) {
                    activity.unregisterReceiver(this);
                    UsbDevice device = (UsbDevice)intent.getParcelableExtra(UsbManager.EXTRA_DEVICE);

                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) {
                        if(device != null){
                            if(connectToDevice()){
                                handler.handleConnected();
                            } else {
                                handler.handleError();
                            }
                        }
                    }
                    else {
                        handler.handleError();
                    }
                }

            }
        }

    }

    protected boolean connectToDevice(){
        iface = device.getInterface(0);
        ep = iface.getEndpoint(0);
        connection = manager.openDevice(device);
        if(connection == null){
            return false;
        }

        if(!connection.claimInterface(iface, true)){
            return false;
        }

        return true;
    }

    protected UsbDevice getDevice(){

        HashMap<String, UsbDevice> deviceList = manager.getDeviceList();

        for(UsbDevice tmpdevice : deviceList.values()){
            if(tmpdevice.getVendorId() == usb_vid && tmpdevice.getProductId() == usb_pid){
                // Found it!
                return tmpdevice;

            }
        }
        return null;
    }

    public void simpletest(UsbManager m, UsbDevice u, PendingIntent pi){
        m.requestPermission(u, pi);
    }


    public int connect(USBConnHandler handler){
        boolean success;

        this.handler = handler;

        IntentFilter filter = new IntentFilter();
        filter.addAction(ACTION_USB_PERMISSION);

        PendingIntent pendingIntent = PendingIntent.getBroadcast(activity, 0,
                                                                 new Intent(ACTION_USB_PERMISSION),
                                                                 0);

        error = "";

        device = getDevice();

        if(device == null){
            return -1;
        }

        UsbReceiver mUsbReceiver = new UsbReceiver();

        if(!manager.hasPermission(device)){
            Log.d("USB", "requestPermission");
            activity.registerReceiver(mUsbReceiver, filter);
            manager.requestPermission(device, pendingIntent);
        } else {
            Log.d("USB", "Already have permission");
            if(connectToDevice()){
                handler.handleConnected();
            } else {
                return -2;
            }
        }



        return 0;
    }

    public int send(byte[] data){
        int count;
        count = connection.controlTransfer(RequestType.DIR_OUT |
                                           RequestType.RECIPIENT_DEVICE |
                                           RequestType.TYPE_CLASS,
                                           HIDVals.SET_REPORT,
                                           (HIDReportType.OUTPUT<<8) | 0x00,
                                           0, data, data.length, 0);
        return count;
    }

    public int receive(byte[] data, int len){
        int count;
        count = connection.controlTransfer(RequestType.DIR_IN |
                                           RequestType.RECIPIENT_INTERFACE |
                                           RequestType.TYPE_CLASS,
                                           HIDVals.GET_REPORT,
                                           (HIDReportType.INPUT<<8) | 0x00,
                                           0, data, len, 0);
        return count;
    }


    private class RequestType {
        public static final int DIR_IN = 0x80;
        public static final int DIR_OUT = 0x00;

        public static final int TYPE_STANDARD = (0x00 << 5);
        // Class
        public static final int TYPE_CLASS = (0x01 << 5);
        // Vendor
        public static final int TYPE_VENDOR = (0x02 << 5);
        // Reserved
        public static final int TYPE_RESERVED = (0x03 << 5);

        public static final int RECIPIENT_DEVICE = 0x00;
        // Interface
        public static final int RECIPIENT_INTERFACE = 0x01;
        // Endpoint
        public static final int RECIPIENT_ENDPOINT = 0x02;
        // Other
        public static final int RECIPIENT_OTHER = 0x03;

    }

    private class HIDVals {
        public static final int GET_REPORT           =     0x01;
        public static final int GET_IDLE             =     0x02;
        public static final int GET_PROTOCOL         =     0x03;
        public static final int SET_REPORT           =     0x09;
        public static final int SET_IDLE             =     0x0A;
        public static final int SET_PROTOCOL         =     0x0B;

    }

    private class HIDReportType {
        public static final int INPUT    =     0x01;
        public static final int OUTPUT   =     0x02;
        public static final int FEATURE  =     0x03;
    }

    public static final String ACTION_USB_PERMISSION =
            "com.audiopulse.usbiface.USB_PERMISSION";

    public String error;

    protected UsbDevice device;
    protected UsbInterface iface;
    protected UsbEndpoint ep;
    protected UsbDeviceConnection connection;
}
