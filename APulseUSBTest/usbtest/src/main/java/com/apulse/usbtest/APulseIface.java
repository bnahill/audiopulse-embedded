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
        usb.receive(buffer.array(), 2);
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
        } while(!(status.reset_controller &&
                  status.reset_input &&
                  status.reset_wavegen));
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
        usb.send(buffer.array(), buffer.position());
    }

    /*!
     @brief A convenient access class for a returned status code
     */
    static public class APulseStatus {
        public APulseStatus(ByteBuffer buffer){
            version = ((int)buffer.getChar(0)) & 0xFF;
            char flags = buffer.getChar(1);
            is_capturing = (flags & (1 << 5)) != 0;
            is_playing = (flags & (1 << 4)) != 0;
            is_started = (flags & (1 << 3)) != 0;
            reset_wavegen = (flags & (1 << 2)) != 0;
            reset_input = (flags & (1 << 1)) != 0;
            reset_controller = (flags & (1 << 0)) != 0;
        }

        public int version;

        public boolean is_capturing;
        public boolean is_playing;
        public boolean is_started;
        public boolean reset_wavegen;
        public boolean reset_input;
        public boolean reset_controller;
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
            IntBuffer iBuffer = buffer.asIntBuffer();

            if(frame_count < psd_frames - 1){
                for(int i = 0; i < 64; i++){
                    psd[frame_count * 16 + i] = iBuffer.get(i);
                }
                // Check if the next one is the wuss-frame (one sample)
                if(frame_count == psd_frames - 2){
                    ret = 4;
                } else {
                    ret = 64;
                }
            } else if (frame_count == psd_frames - 1){
                // This is the last PSD frame, containing only one sample
                psd[frame_count * 16] = iBuffer.get(0);
                ret = 64;
            } else if (frame_count - psd_frames < average_frames){
                // Now receiving average frames
                for(int i = 0; i < 64; i++){
                    average[(frame_count - psd_frames)* 16 + i] = iBuffer.get(i);
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

        public final int[] getPSD(){return psd;}
        public final int[] getAverage(){return average;}

        private int frame_count;
        private int average[];
        private int psd[];

        private static final int psd_frames =
                (int)Math.ceil((float)(APulseIface.transform_len / 2 + 1) / 16);
        private static final int average_frames =
                (int)((APulseIface.transform_len) / 16);
    }

    public class ToneConfig {
        public void toBuffer(ByteBuffer buffer){
            buffer.put((byte)((tone << 4) & ch));
            buffer.putShort((short)(db * (float)(1 << 8)));
            buffer.putShort(f1);
            buffer.putShort(f2);
            buffer.putShort(t1);
            buffer.putShort(t2);
        }

        protected int tone;
        protected short f1;
        protected short f2;
        protected short t1;
        protected short t2;
        protected float db;
        protected int ch;
    }

    public class FixedTone extends ToneConfig {
        public FixedTone(short f1, short t1, short t2, float db, int ch){
            this.tone = TONE_FIXED;
            this.f1 = f1;
            this.f2 = f1;
            this.t1 = t1;
            this.t2 = t2;
            this.db = db;
            this.ch = ch;
        }

    }

    public class ChirpTone extends ToneConfig {
        public ChirpTone(short f1, short f2, short t1, short t2, float db, int ch){
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

    private USBIface usb;

    private static final int CMD_RESET        = 0;
    private static final int CMD_STARTUP      = 1;
    private static final int CMD_SETUPTONES   = 2;
    private static final int CMD_SETUPCAPTURE = 3;
    private static final int CMD_GETSTATUS    = 4;
    private static final int CMD_GETDATA      = 5;
    private static final int CMD_START        = 6;

    private static final int TONE_OFF         = 0;
    private static final int TONE_FIXED       = 0;
    private static final int TONE_CHIRP       = 0;
}
