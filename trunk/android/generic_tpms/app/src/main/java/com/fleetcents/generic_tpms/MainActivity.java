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
import android.util.Log;
import android.view.animation.Animation;
import android.view.animation.AlphaAnimation;
import android.view.animation.LinearInterpolator;
import android.view.View.OnClickListener;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.Volley;
import com.android.volley.toolbox.StringRequest;

public class MainActivity extends Activity implements Runnable {

    private static final int RTL2832U_RESULT_CODE = 1234;  // arbitrary value, used when sending intent to RTL2832U
    private static final String TMPFCBINFN = "fc.bin";
    private static final String LOGTAG = "MainActivity";
    private static final String TAG = "FleetCents";
    private MenuItem mi_startStop = null;
    private boolean running = false;

    public static native void convertAndGetPkt();

    public static native double getKpa();

    public static native double getPsi();

    public static native long getTempF();

    public static native long getTempC();

    public static native long getDecAddr();

    public static native String getHexAddr(String str);

    public static native String getFullUrl(String str);

    private TextView myText = null;

    static {
        try {
            System.loadLibrary("fleetcents");
            Log.i(TAG, "Successfully loaded native fleetcents library");
        } catch (Throwable t) {
            Log.w(TAG, "Failed to load native library:" + t.getMessage(), t);
        }
    }

    private String cacheDir() {
        return this.getCacheDir().getAbsolutePath();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        running = false;
        File file = new File(this.cacheDir(), TMPFCBINFN);
        file.delete();

        LinearLayout lView = new LinearLayout(this);

        myText = new TextView(this);
        myText.setText("Press the play button above to generate an activation tone " +
                "and begin receiving data from your TPMS sensor for processing.\n\n\n");

        lView.addView(myText);

        setContentView(lView);

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main_menu, menu);
        // Get a reference to the start-stop button:
        mi_startStop = menu.findItem(R.id.action_startStop);
        mi_startStop.setIcon(R.drawable.ic_action_play);

        // update the action bar icons and titles according to the app state:
        updateActionBar();
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle presses on the action bar items
        switch (item.getItemId()) {
            case R.id.action_startStop:
                if (!running) {
                    running = !running;
                    updateActionBar();
                    run();
                }
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    private void updateActionBar() {
        // Set icon of the start/stop button according to the state:
        if (mi_startStop != null) {
            final Button btn = (Button) findViewById(R.id.action_startStop);
            if (btn == null) {
                Log.w(TAG, "btn NOT FOUND");
            } else {
                Log.i(TAG, "btn IS FOUND");
            }
            if (running) {
                final Animation animation = new AlphaAnimation(1, 0); // Change alpha from fully visible to invisible
                animation.setDuration(150); // duration - half a second
                animation.setInterpolator(new LinearInterpolator()); // do not alter animation rate
                animation.setRepeatCount(Animation.INFINITE); // Repeat animation infinitely
                animation.setRepeatMode(Animation.REVERSE); // Reverse animation at the end so the button will fade back in
                btn.startAnimation(animation);
            } else {
                btn.clearAnimation();
            }
        }
    }

    /*
     * Method called by the native code to get a device handle
     */
    public UsbDeviceConnection open(String device_name) {
        Log.d(TAG, "Open called " + device_name);
        return null;
    }

    void setDevice(UsbDevice device) {
//    if (myNativeThread == null) {
//      myNativeThread = new Thread(this);
//      myNativeThread.start();
//    }
    }

    @Override
    public void run() {
        myText.append("Sending Activation Signal\n");
        try {
            Intent intent = new Intent(Intent.ACTION_VIEW);
            File file = new File(this.cacheDir(), TMPFCBINFN);
            myText.append("Waiting for Sensor Data\n");
            intent.setData(Uri.parse("iqsrc://-f 315000000 -s 2048000 -n 4000000 \"" + file.getAbsolutePath() + "\""));
            myText.append("Receiving Sensor Data\n");
            startActivityForResult(intent, RTL2832U_RESULT_CODE);
        } catch (ActivityNotFoundException e) {
            Log.e(LOGTAG, "createSource: RTL2832U is not installed");

            // Show a dialog that links to the play market:
            new AlertDialog.Builder(this)
                    .setTitle("RTL2832U driver not installed!")
                    .setMessage("You need to install the (free) RTL2832U driver to use RTL-SDR dongles.")
                    .setPositiveButton("Install from Google Play", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            Intent marketIntent = new Intent(Intent.ACTION_VIEW, Uri.parse("market://details?id=marto.rtl_tcp_andro"));
                            startActivity(marketIntent);
                        }
                    })
                    .setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int whichButton) {
                            // do nothing
                        }
                    })
                    .show();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        // err_info from RTL2832U:
        String[] rtlsdrErrInfo = {
                "permission_denied",
                "root_required",
                "no_devices_found",
                "unknown_error",
                "replug",
                "already_running"};
        File file = new File(this.cacheDir(), TMPFCBINFN);

        switch (requestCode) {
            case RTL2832U_RESULT_CODE:
                // This happens if the RTL2832U driver was started.
                // We check for errors and print them:
                if (resultCode == RESULT_OK) {
                    Log.i(LOGTAG, "onActivityResult: RTL-SDR Data Capture Completed.");
                    convertAndGetPkt();
                    file.delete();
                    String hxAddr = null;
                    getHexAddr(hxAddr);
                    myText.append("Sensor ID:" + hxAddr + "\n");
                    long tmpC = getTempC();
                    myText.append("Temperature: " + String.valueOf(tmpC) + " deg C\n");
                    double press_psi = getPsi();
                    myText.append("Pressure: " + String.valueOf(press_psi) + " (psi)\n");
                    myText.append("Sending Sensor Data to Fleet Cents\n");

                    // Instantiate the RequestQueue.
                    RequestQueue queue = Volley.newRequestQueue(this);
                    String url = null;
                    getFullUrl(url);
                    // Request a string response from the provided URL.
                    StringRequest stringRequest = new StringRequest(StringRequest.Method.GET, url,
                            new Response.Listener<String>() {
                                @Override
                                public void onResponse(String response) {
                                    myText.append("Sensor Data Received OK\n");

                                }
                            }, new Response.ErrorListener() {
                        @Override
                        public void onErrorResponse(VolleyError error) {
                            myText.append("Sensor Data Not Stored OK");
                        }
                    });
                    // Add the request to the RequestQueue.
                    queue.add(stringRequest);
                } else {
                    myText.append("An Unknown Error Has Occurred");
                    int errorId = -1;
                    int exceptionCode = 0;
                    String detailedDescription = null;
                    if (data != null) {
                        errorId = data.getIntExtra("com.fleetcents.generic_tpms.RtlTcpExceptionId", -1);
                        exceptionCode = data.getIntExtra("detailed_exception_code", 0);
                        detailedDescription = data.getStringExtra("detailed_exception_message");
                    }
                    String errorMsg = "ERROR NOT SPECIFIED";
                    if (errorId >= 0 && errorId < rtlsdrErrInfo.length)
                        errorMsg = rtlsdrErrInfo[errorId];

                    Log.e(LOGTAG, "onActivityResult: RTL2832U driver returned with error: " + errorMsg + " (" + errorId + ")"
                            + (detailedDescription != null ? ": " + detailedDescription + " (" + exceptionCode + ")" : ""));

                }
                break;
        }
        running = false;
        updateActionBar();
    }
}