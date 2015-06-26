package com.fleetcents.generic_tpms;

import java.io.File;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ActivityNotFoundException;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.os.Bundle;
import android.net.Uri;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.AlphaAnimation;
import android.view.animation.LinearInterpolator;
import android.view.View.OnClickListener;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.LinearLayout.LayoutParams;

import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.Volley;
import com.android.volley.toolbox.StringRequest;

import static android.support.v4.content.LocalBroadcastManager.*;

public class MainActivity extends Activity implements Runnable {

    private static final int RTL2832U_RESULT_CODE = 1234;  // arbitrary value, used when sending intent to RTL2832U
    private static final String TMPFCBINFN = "fc.bin";
    private static final String LOGTAG = "MainActivity";
    private static final String TAG = "FleetCents";
    private static final MainActivity sole_instance = null;
    private MenuItem mi_startStop = null;
    private boolean running = false;
    private boolean testing = true;

    public static native int pktFound();

    public static native void convertAndGetPkt();

    public static native double getKpa();

    public static native double getPsi();

    public static native long getTempF();

    public static native long getTempC();

    public static native long getDecAddr();

    public static native String getHexAddr();

    public static native String getFullUrl();

    public static native void setCacheDir(String str);

    private TextView myText = null;

    static {
        try {
            // Log.i(TAG, "Trying to load native fleetcents library");
            System.loadLibrary("fleetcents");
            Log.i(TAG, "Successfully loaded native fleetcents library");
        } catch (Throwable t) {
            Log.w(TAG, "Failed to load native library:" + t.getMessage(), t);
        }
    }

    private String cacheDir() {
        // emulator expects dir at /storage/sdcard/Android/data/com.fleetcents.generic_tpms/cache
        Log.i(TAG, "Enter cacheDir -- caching " + String.valueOf(this.getExternalCacheDir()));
    	setCacheDir(String.valueOf(this.getExternalCacheDir()));
        Log.i(TAG, "Exit cacheDir");
        return String.valueOf(this.getExternalCacheDir());
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(TAG, "Enter onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        myText = new TextView(this);
        myText.setText(getString(R.string.hello_world));
        myText.setInputType(0x00020001); // multi-line text box
        LinearLayout linearLayout = (LinearLayout) findViewById(R.id.main_activity);
        linearLayout.addView(myText, new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));

        running = false;
        if (!testing) {
            File file = new File(cacheDir(), TMPFCBINFN);
            // Emulator looks for temporary file at /data/data/com.fleetcents.generic_tpms/cache/fc.bin
            // Need a rw directory, pref on external storage
            // Would generally prefer putting this in external storage
            Log.i(TAG, "Looking for temporary file at " + file.getAbsolutePath());
            file.delete();
        }
        else {
            cacheDir();
        }

        Log.i(TAG, "Exit onCreate");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.i(TAG, "Enter onCreateOptionsMenu");

        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main_menu, menu);
        // Get a reference to the start-stop button:
        mi_startStop = menu.findItem(R.id.action_startStop);
        mi_startStop.setActionView(null);

        Log.i(TAG, "Exit onCreateOptionsMenu");
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Log.i(TAG, "Enter onOptionsItemSelected");
        // Handle presses on the action bar items
        switch (item.getItemId()) {
            case R.id.action_startStop:
                if (!running) {
                    running = true;
                    updateActionBarAnimation();
                    if (testing) {
                        processRtlsdrData();
                        running = false;
                        updateActionBarAnimation();
                    }
                    else {
                        run();
                    }
                }
                Log.i(TAG, "Exit onOptionsItemSelected");
                return true;
            default:
                Log.i(TAG, "Exit onOptionsItemSelected");
                return super.onOptionsItemSelected(item);
        }
    }

    private void updateActionBarAnimation() {
        Log.i(TAG, "Enter updateActionBarAnimation");
        // Set icon of the start/stop button according to the state:
        if (mi_startStop != null) {
            if (running) {
            	mi_startStop.setActionView(R.layout.iv_refresh);
            } else {
                mi_startStop.setActionView(null);
            }
        }
        Log.i(TAG, "Exit updateActionBarAnimation");
    }

    @Override
    public void run() {
        Log.i(TAG, "Enter run");
        myText.append(getString(R.string.msg_send_activation) + "\n");

        Intent intent = new Intent(Intent.ACTION_VIEW);

        File file = new File(cacheDir(), TMPFCBINFN);

        myText.append(getString(R.string.msg_wait_for_sensor_data) + "\n");
        intent.setData(Uri.parse("iqsrc://-f 315000000 -s 2048000 -n 4000000 \"" + file.getAbsolutePath() + "\""));

        myText.append(getString(R.string.msg_rcving_sensor_data) + "\n");
//            startActivityForResult(intent, RTL2832U_RESULT_CODE);
        try {
            LocalBroadcastManager.getInstance(MainActivity.this).sendBroadcastSync(intent);
            Log.i(LOGTAG, "onActivityResult: RTL-SDR Data Capture Completed.");
            processRtlsdrData();
        }
        catch (Exception e) {
            myText.append(getString(R.string.msg_unknown_error));
        }

        running = false;
        updateActionBarAnimation();
        Log.i(TAG, "Exit run");
    }

    private void processRtlsdrData() {
        Log.i(TAG, "Enter processRtlsdrData");
        convertAndGetPkt();
        // file.delete();
        if (pktFound() == 1) {
            String hxAddr = getHexAddr();
            Log.i(TAG, "hxAddr is >" + hxAddr + "<");
            myText.append(getString(R.string.sensor_id) + " " + hxAddr + "\n");

            long tmpC = getTempC();
            Log.i(TAG, "tmpC is >" + (double)(tmpC) + "<");
            myText.append(getString(R.string.temp) + " " + ((double)((int)(tmpC*10)))/10 + getString(R.string.celsius) + "\n");

            double press_psi = getPsi();
            Log.i(TAG, "press_psi is >" + press_psi + "<");
            myText.append(getString(R.string.pressure) + " " + ((double)((int)(press_psi*100)))/100 + getString(R.string.psi) + "\n");

            myText.append(getString(R.string.msg_sending_data) + "\n");

            // Instantiate the RequestQueue.
            RequestQueue queue = Volley.newRequestQueue(this);
            String url = getFullUrl();
            Log.i(TAG, "url is >" + url + "<");
            // Request a string response from the provided URL.
            StringRequest stringRequest = new StringRequest(StringRequest.Method.GET, url,
                    new Response.Listener<String>() {
                        @Override
                        public void onResponse(String response) {
                            myText.append(getString(R.string.msg_data_rcvd_ok) + "\n");

                        }
                    }, new Response.ErrorListener() {
                @Override
                public void onErrorResponse(VolleyError error) {
                    myText.append(getString(R.string.msg_data_not_rcvd_ok) + "\n");
                }
            });
            // Add the request to the RequestQueue.
            queue.add(stringRequest);
        }
        else {
            myText.append(getString(R.string.msg_valid_pkt_not_found) + "\n");
        }
        Log.i(TAG, "Exit processRtlsdrData");
    }
}