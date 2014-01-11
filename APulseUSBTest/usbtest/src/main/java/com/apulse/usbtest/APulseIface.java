package com.apulse.usbtest;

import android.app.Activity;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.IntBuffer;


/**
 * Created by ben on 1/3/14.
 */
public class APulseIface {
    public APulseIface(Activity activity) {
        this.usb = new USBIface(activity);
        buffer = ByteBuffer.allocateDirect(64);
        buffer.order(ByteOrder.LITTLE_ENDIAN);
    }

    public int connect(USBIface.USBConnHandler handler){
        return usb.connect(handler);
    }

    public APulseStatus getStatus(){
        usb.receive(buffer.array(), 5);
        return new APulseStatus(buffer);
    }

    /*!
     @brief Reset the device and wait for it to confirm
     */
    public void reset(){
        APulseStatus status;
        buffer.array()[0] = CMD_RESET;
        usb.send(buffer.array(), 1);
        do{
            status = getStatus();
        } while(!((status.test_state == APulseStatus.TEST_RESET) &&
                  (status.wg_state == APulseStatus.WG_RESET) &&
                  (status.in_state == APulseStatus.IN_RESET)));
    }

    /*!
     @brief Get PSD and average data from device

     @note This MUST be preceded by a getStatus call confirming that all captures are done
     */
    public APulseData getData(){
        APulseData data = new APulseData();
        buffer.array()[0] = CMD_GETDATA;
        usb.send(buffer.array(), 1);
        // Initially read 64
        int len = 64;
        while(len != 0){
            usb.receive(buffer.array(), len);
            len = data.pushBuffer(buffer);
        }
        return data;
    }

    public void configTones(ToneConfig[] tones){
        buffer.position(0);
        buffer.put((byte)CMD_SETUPTONES);
        for(int i = 0; i < 3; i++){
            if(tones.length > i){
                tones[i].toBuffer(buffer);
            } else {
                ToneOff t = new ToneOff();
                t.toBuffer(buffer);
            }
        }
        buffer.position(0);
        usb.send(buffer.array(), 35);
    }

    public void configCapture(int t1, int overlap, int nframes){
        buffer.position(0);
        buffer.put((byte) CMD_SETUPCAPTURE);
        buffer.putShort((short) overlap);
        buffer.put((byte) 0); //window function, ignore it
        buffer.putShort((short) nframes);
        buffer.putShort((short) t1);
        buffer.position(0);
        usb.send(buffer.array(), 63);
    }

    public void start(){
        buffer.position(0);
        buffer.put((byte)CMD_START);

        buffer.position(0);
        usb.send(buffer.array(), 63);
    }

    /*!
     @brief A convenient access class for a returned status code
     */
    static public class APulseStatus {
        public APulseStatus(ByteBuffer buffer){
            version = (int)buffer.get(0) & 0xFF;
            in_state = (int)buffer.get(1) & 0xFF;
            wg_state = (int)buffer.get(2) & 0xFF;
            test_state = (int)buffer.get(3) & 0xFF;
            err_code = (int)buffer.get(4) & 0xFF;
        }

        public int version;
        public int wg_state;
        public int in_state;
        public int test_state;
        public int err_code;

        public static final int WG_RESET   = 0;
        public static final int WG_READY   = 1;
        public static final int WG_RUNNING = 2;
        public static final int WG_DONE    = 3;

        public static final int IN_RESET     = 0;
        public static final int IN_READY     = 1;
        public static final int IN_RUNWAIT   = 2;
        public static final int IN_CAPTURING = 3;
        public static final int IN_DONE      = 4;

        public static final int TEST_RESET       = 0;
        public static final int TEST_CONFIGURING = 1;
        public static final int TEST_READY       = 2;
        public static final int TEST_RUNNING     = 3;
        public static final int TEST_DONE        = 4;


        public String wgStateString(){
            switch(wg_state){
                case WG_RESET:
                    return "reset";
                case WG_READY:
                    return "ready";
                case WG_RUNNING:
                    return "running";
                case WG_DONE:
                    return "done";
                default:
                    return "unknown";
            }
        }

        public String testStateString(){
            switch(test_state){
                case TEST_RESET:
                    return "reset";
                case TEST_CONFIGURING:
                    return "configuring";
                case TEST_READY:
                    return "ready";
                case TEST_RUNNING:
                    return "running";
                case TEST_DONE:
                    return "done";
                default:
                    return "unknown";
            }
        }

        public String inStateString(){
            switch(in_state){
                case IN_RESET:
                    return "reset";
                case IN_READY:
                    return "ready";
                case IN_RUNWAIT:
                    return "runwait";
                case IN_CAPTURING:
                    return "capturing";
                case IN_DONE:
                    return "done";
                default:
                    return "unknown";
            }
        }
    }

    /*!
     @brief A representation of data from the embedded device which also provides
        parsing facilities
     */
    static public class APulseData {
        public APulseData() {
            frame_count = 0;
            average = new int[APulseIface.transform_len];
            psd = new int[APulseIface.transform_len / 2 + 1];
        }

        /*!
         @brief Apply a new received buffer
         @param buffer The received buffer
         @return The number of bytes to receive for the next frame

         @note Call pushBuffer until it returns 0
         */
        public int pushBuffer(ByteBuffer buffer){
            int ret;
            buffer.position(0);

            if(frame_count < psd_frames - 1){
                for(int i = 0; i < 16; i++){
                    psd[frame_count * 16 + i] = buffer.getInt();//iBuffer.get(i);
                }
                // Check if the next one is the wuss-frame (one sample)
                if(frame_count == psd_frames - 2){
                    ret = 4;
                } else {
                    ret = 64;
                }
            } else if (frame_count == psd_frames - 1){
                // This is the last PSD frame, containing only one sample
                psd[frame_count * 16] = buffer.getInt();//iBuffer.get(0);
                ret = 64;
            } else if (frame_count - psd_frames < average_frames){
                // Now receiving average frames
                for(int i = 0; i < 16; i++){
                    average[(frame_count - psd_frames)* 16 + i] = buffer.getInt();//iBuffer.get(i);
                }
                // Check if last frame
                if(frame_count - psd_frames == average_frames - 1)
                    ret = 0;
                else
                    ret = 64;
            } else {
                // Why are we here?
                ret = 0;
            }

            frame_count += 1;

            return ret;
        }

        //public final int[] getPSD(){return psd;}
        //public final int[] getAverage(){return average;}
        public double[] getPSD(){
            double[] ret = new double[APulseIface.transform_len / 2 + 1];
            for(int i = 0; i < APulseIface.transform_len / 2 + 1; i++){
                ret[i] = ((double)psd[i]) / (double)0x7FFFFFFF;
            }
            return ret;
        }

        public double[] getAverage(){
            double[] ret = new double[APulseIface.transform_len];
            for(int i = 0; i < APulseIface.transform_len; i++){
                ret[i] = ((double)average[i]) / (double)0x7FFFFFFF;
            }
            return ret;
        }

        private int frame_count;
        private int average[];
        private int psd[];

        private static final int psd_frames =
                (int)Math.ceil((float)(APulseIface.transform_len / 2 + 1) / 16);
        private static final int average_frames =
                (int)((APulseIface.transform_len) / 16);
    }

    public static class ToneConfig {
        public void toBuffer(ByteBuffer buffer){
            buffer.put((byte)((ch << 4) | tone));
            buffer.putShort((short)(db * (float)(1 << 8)));
            buffer.putShort((short)f1);
            buffer.putShort((short)f2);
            buffer.putShort((short)t1);
            buffer.putShort((short)t2);
        }

        protected int tone;
        protected int f1;
        protected int f2;
        protected int t1;
        protected int t2;
        protected double db;
        protected int ch;
    }

    public static class FixedTone extends ToneConfig {
        public FixedTone(int f1, int t1, int t2, double db, int ch){
            this.tone = TONE_FIXED;
            this.f1 = f1;
            this.f2 = f1;
            this.t1 = t1;
            this.t2 = t2;
            this.db = db;
            this.ch = ch;
        }

    }

    public static class ChirpTone extends ToneConfig {
        public ChirpTone(int f1, int f2, int t1, int t2, double db, int ch){
            this.tone = TONE_CHIRP;
            this.f1 = f1;
            this.f2 = f2;
            this.t1 = t1;
            this.t2 = t2;
            this.db = db;
            this.ch = ch;
        }
    }

    public class ToneOff extends ToneConfig {
        public ToneOff(){
            this.tone = TONE_OFF;
        }
    }

    public static final int transform_len = 512;

    private ByteBuffer buffer;

    public USBIface usb;

    private static final int CMD_RESET        = 0;
    private static final int CMD_STARTUP      = 1;
    private static final int CMD_SETUPTONES   = 2;
    private static final int CMD_SETUPCAPTURE = 3;
    private static final int CMD_GETSTATUS    = 4;
    private static final int CMD_GETDATA      = 5;
    private static final int CMD_START        = 6;

    private static final int TONE_OFF         = 0;
    private static final int TONE_FIXED       = 1;
    private static final int TONE_CHIRP       = 2;
}
