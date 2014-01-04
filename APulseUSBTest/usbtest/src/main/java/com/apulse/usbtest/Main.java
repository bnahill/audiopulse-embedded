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

        textsend = (EditText) findViewById(R.id.editText);
        textrecv = (EditText) findViewById(R.id.editText2);

        send_button.setEnabled(false);
        recv_button.setEnabled(false);

        usb = new USBIface(this);


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
        int size = usb.receive(data, 10);
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
        int cnt = usb.send(bytes);
        if(cnt >= 0){
            textview.setText("Sent data! -- " + cnt);
        } else {
            textview.setText("Error sending data! -- " + cnt);
        }
    }

    public void connectToggleButton(View view){
        Switch sw = (Switch)view;
        int ret;
        if(sw.isChecked()){
            textview.setText("Attempting to connect...");
            ret = usb.connect(new USBIface.USBConnHandler() {
                public void handleConnected(){
                    textview.setText("Connected successfully!");
                    send_button.setEnabled(true);
                    recv_button.setEnabled(true);
                }
                public void handleError(){
                    textview.setText("Error with permissions");
                    send_button.setEnabled(false);
                    recv_button.setEnabled(false);
                }
            });

            if(ret != 0){
                textview.setText(String.format("Error connecting: %d ",ret) + usb.error);
            }
        } else {
            // Ignore for now
            textview.setText("Disconnected!");
        }
    }

    protected Button send_button;
    protected Button send_clear_button;
    protected Button recv_button;
    protected Button recv_clear_button;

    protected TextView textview;

    protected Switch toggle_button;

    protected EditText textsend, textrecv;

    protected USBIface usb;
}
