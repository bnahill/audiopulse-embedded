package com.apulse.usbtest;

import android.app.Activity;
import android.app.Fragment;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;


public class Main extends Activity {
    private final BroadcastReceiver mUsbReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            Log.d("musbreceiver", "activity received broadcast");
            if (USBIface.ACTION_USB_PERMISSION.equals(action)) {

            }
        }

    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        if (savedInstanceState == null) {
            getFragmentManager().beginTransaction()
                    .add(R.id.container, new PlaceholderFragment())
                    .commit();
        }

        send_button = (Button)findViewById(R.id.button);
        send_clear_button = (Button)findViewById(R.id.button2);
        recv_button = (Button)findViewById(R.id.button3);
        recv_clear_button = (Button)findViewById(R.id.button4);

        textview = (TextView) findViewById(R.id.textView);
        toggle_button = (Switch) findViewById(R.id.switch1);

        textsend = (EditText) findViewById(R.id.editText2);
        textrecv = (EditText) findViewById(R.id.editText);
        app_out = (EditText) findViewById(R.id.editText3);

        reset_button = (Button)findViewById(R.id.button5);
        status_button = (Button)findViewById(R.id.button6);
        start_button = (Button)findViewById(R.id.button7);
        getdata_button = (Button)findViewById(R.id.button8);

        send_button.setEnabled(false);
        recv_button.setEnabled(false);

        status_button.setEnabled(false);
        getdata_button.setEnabled(false);
        reset_button.setEnabled(false);
        start_button.setEnabled(false);

        apulse = new APulseIface(this);


        app_out.setKeyListener(null);
        textrecv.setKeyListener(null);

        //IntentFilter filter = new IntentFilter(ACTION_USB_PERMISSION);
        //registerReceiver(mUsbReceiver, filter);
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }

    /**
     * A placeholder fragment containing a simple view.
     */
    public static class PlaceholderFragment extends Fragment {

        public PlaceholderFragment() {
        }

        @Override
        public View onCreateView(LayoutInflater inflater, ViewGroup container,
                Bundle savedInstanceState) {
            View rootView = inflater.inflate(R.layout.fragment_main, container, false);
            return rootView;
        }
    }

    public void recvClearButton(View view){
        textrecv.setText("");
    }

    public void recvButton(View view){
        byte[] data = new byte[10];//USBIface.max_transfer_size];
        int size = apulse.usb.receive(data, 10);
        String s;
        try{
            s = new String(data, "UTF-8");
        } catch(Exception e){
            s = "Wrong encoding?";
        }
        if(size >= 0){
            textrecv.setText("test:"+s+":");
            textview.setText(String.format("Read %d bytes!",size));
        } else {
            textrecv.setText("");
            textview.setText(String.format("Error reading! -- %d",size));
        }
    }

    public void sendClearButton(View view){
        textsend.setText("");
    }

    public void sendButton(View view){
        byte[] bytes = textsend.getText().toString().getBytes();
        int cnt = apulse.usb.send(bytes, bytes.length);
        if(cnt >= 0){
            textview.setText("Sent data! -- " + cnt);
        } else {
            textview.setText("Error sending data! -- " + cnt);
        }
    }

    public void connectToggleButton(View view){
        final Switch sw = (Switch)view;
        int ret;
        app_out.setText("");
        if(sw.isChecked()){
            textview.setText("Attempting to connect...");
            ret = apulse.usb.connect(new USBIface.USBConnHandler() {
                public void handleConnected(){
                    textview.setText("Connected successfully!");
                    send_button.setEnabled(true);
                    recv_button.setEnabled(true);
                    status_button.setEnabled(true);
                    getdata_button.setEnabled(true);
                    reset_button.setEnabled(true);
                    start_button.setEnabled(true);
                }
                public void handleError(){
                    textview.setText("Error with permissions");
                    sw.setChecked(false);

                    send_button.setEnabled(false);
                    recv_button.setEnabled(false);

                    status_button.setEnabled(false);
                    getdata_button.setEnabled(false);
                    reset_button.setEnabled(false);
                    start_button.setEnabled(false);
                }
            });

            if(ret != 0){
                textview.setText(String.format("Error connecting: %d ",ret) + apulse.usb.error);
                sw.setChecked(false);
            }
        } else {
            // Ignore for now
            textview.setText("Disconnected!");
        }
    }

    public void resetButton(View view){
        apulse.reset();
    }

    public void statusButton(View view){
        APulseIface.APulseStatus status = apulse.getStatus();

        String out = String.format(
                "Version:       %d\n" +
                "Test state:    %s\n" +
                "WaveGen state: %s\n" +
                "Input state:   %s\n" +
                "Error:         %d",
                status.version,
                status.testStateString(),
                status.wgStateString(),
                status.inStateString(),
                status.err_code);
        app_out.setText(out);
    }

    public void startButton(View view){
        APulseIface.ToneConfig[] tones = new APulseIface.ToneConfig[2];
        tones[0] = new APulseIface.FixedTone(500, 1000, 10000, 65.0, 0);
        tones[1] = new APulseIface.FixedTone(1000, 1000, 10000, 65.0, 1);

        apulse.configCapture(2000, 256, 200);

        apulse.configTones(tones);

        apulse.start();
    }

    public void getdataButton(View view){
        if(apulse.getStatus().test_state == APulseIface.APulseStatus.TEST_DONE){
            APulseIface.APulseData data = apulse.getData();
            double[] psd = data.getPSD();
            String out = "";
            for(int i = 0; i < psd.length; i++){
                double logd = (psd[i] == 0) ? -Double.NEGATIVE_INFINITY : Math.log10(psd[i]);

                out += String.format("%d:\t%.10f\n",(int)(((double)i)*31.25), logd);
            }
            app_out.setText(out);
        } else {
            app_out.setText("Data not ready...");
        }
    }

    protected Button send_button;
    protected Button send_clear_button;
    protected Button recv_button;
    protected Button recv_clear_button;

    protected Button reset_button;
    protected Button status_button;
    protected Button start_button;
    protected Button getdata_button;


    protected TextView textview;

    protected EditText app_out;

    protected Switch toggle_button;

    protected EditText textsend, textrecv;

    protected APulseIface apulse;
}
