package com.fleetcents.generic_tpms;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.HashSet;

import android.app.Activity;
import android.app.DialogFragment;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.net.Uri;
import android.support.v4.app.FragmentActivity;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.LinearLayout.LayoutParams;
import android.annotation.SuppressLint;
import android.app.PendingIntent;
import android.content.ComponentName;
import android.content.ServiceConnection;

import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.Volley;
import com.android.volley.toolbox.StringRequest;

import com.fleetcents.generic_tpms.R;
import com.fleetcents.generic_tpms.core.DeviceNotFoundDialogFragment;
import com.fleetcents.generic_tpms.core.RtlSdr;
import com.fleetcents.generic_tpms.core.RtlSdrException;
import com.fleetcents.generic_tpms.core.RtlSdrStartException;
import com.fleetcents.generic_tpms.core.UsbHelper;
import com.fleetcents.generic_tpms.core.UsbHelper.STATUS;

public class MainActivity extends Activity implements Runnable {

    // private static final int RTL2832U_RESULT_CODE = 1234;  // arbitrary value, used when sending intent to RTL2832U

    private static final String TMPFCBINFN = "fc.bin";
    private static final String LOGTAG = "MainActivity";
    private static final String TAG = "FleetCents";
    private MenuItem mi_startStop = null;
    private boolean running = false;
    private boolean testing = false;

    private UsbManager mUsbManager = null;

    private String arguments = "";
    private String uspfs_path = null;
    private int fd = -1;

    public static PendingIntent permissionIntent;
    public static Intent intent;

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
            Log.i(LOGTAG, "Loading libraries");
            System.loadLibrary("rtlsdr");
            System.loadLibrary("fleetcents");
            Log.i(LOGTAG, "Successfully loaded native libraries");
        } catch (Throwable t) {
            Log.w(LOGTAG, "Failed to load native libraries");
        }
    }

    private String cacheDir() {
        // emulator expects dir at /storage/sdcard/Android/data/com.fleetcents.generic_tpms/cache
        Log.i(LOGTAG, "Enter cacheDir");
//        String retVal = System.getenv("SECONDARY_STORAGE");
//        if (retVal == null) retVal = System.getenv("EXTERNAL_STORAGE");
//        if (retVal == null) retVal = android.os.Environment.getExternalStorageDirectory().getAbsolutePath();
        File[] dirs = ContextCompat.getExternalFilesDirs(this, "rtlsdr");
        File fdir = dirs[dirs.length-1];
        String retVal = String.valueOf(fdir);
        Log.i(LOGTAG, "cached dir = " + fdir.getPath());
        setCacheDir(retVal);
        File f = new File(fdir, TMPFCBINFN);
        f.mkdirs();
        Log.i(LOGTAG, "Ensuring deletion of " + f.getPath());
        f.delete();

        Log.i(LOGTAG, "Creating " + f.getPath());
        try {
            f.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }
        Log.i(LOGTAG, "Exit cacheDir");
        return retVal;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(LOGTAG, "Enter onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        myText = new TextView(this);
        myText.setText(getString(R.string.hello_world));
        Log.d(LOGTAG, "displaying "+getString(R.string.hello_world));
        myText.setInputType(0x00020001); // multi-line text box
        LinearLayout linearLayout = (LinearLayout) findViewById(R.id.main_activity);
        linearLayout.addView(myText, new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));

        permissionIntent = PendingIntent.getBroadcast(this, 0, new Intent("com.android.example.USB_PERMISSION"), 0);
        mUsbManager = (UsbManager) getSystemService(Context.USB_SERVICE);
        Log.i(LOGTAG, "USB Manager found.");

        running = false;
        if (testing) {
            cacheDir();
        }

        Log.i(LOGTAG, "Exit onCreate");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.i(LOGTAG, "Enter onCreateOptionsMenu");

        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main_menu, menu);
        // Get a reference to the start-stop button:
        mi_startStop = menu.findItem(R.id.action_startstop);
        mi_startStop.setActionView(null);

        Log.i(LOGTAG, "Exit onCreateOptionsMenu");
        return true;
    }


    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Log.i(LOGTAG, "Enter onOptionsItemSelected");
        // Handle presses on the action bar items
        switch (item.getItemId()) {
            case R.id.action_startstop:
                if (!running) {
                    myText.setText(getString(R.string.hello_world));
                    Log.d(LOGTAG, "displaying "+getString(R.string.hello_world));

                    running = true;
                    updateActionBarAnimation();
                    if (testing) {
                        processRtlsdrData();
                        running = false;
                        updateActionBarAnimation();
                    } else {
                        run();
                    }
                }
                Log.i(LOGTAG, "Exit onOptionsItemSelected");
                return true;
            default:
                Log.i(LOGTAG, "Exit onOptionsItemSelected");
                return super.onOptionsItemSelected(item);
        }
    }

    private void updateActionBarAnimation() {
        Log.i(LOGTAG, "Enter updateActionBarAnimation");
        // Set icon of the start/stop button according to the state:
        if (mi_startStop != null) {
            if (running) {
                LayoutInflater inflater = (LayoutInflater) this.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                ImageView imageView = (ImageView) inflater.inflate(R.layout.iv_refresh, null);
                Animation fade = AnimationUtils.loadAnimation(this, R.anim.fade);
                imageView.startAnimation(fade);
                mi_startStop.setActionView(R.layout.iv_refresh);
//                MenuItemCompat.setActionView(mi_startStop, imageView);
            } else {
                mi_startStop.setActionView(null);
            }
        }
        Log.i(LOGTAG, "Exit updateActionBarAnimation");
    }

    @Override
    public void run() {
        Log.i(LOGTAG, "Enter run");
        myText.append("" + getString(R.string.msg_send_activation) + "\n");
        Log.d(LOGTAG, "displaying " + getString(R.string.msg_send_activation));

        File file = new File(cacheDir(), TMPFCBINFN);
        arguments = "-f 315000000 -s 2048000 -n 7500000 -S " + file + "";
        Log.i(LOGTAG, "arguments = >" + arguments + "<");

        UsbDevice device = UsbHelper.findDevice(this);

        final UsbDeviceConnection connection = UsbHelper.openDevice(this, device);
        if (connection == null) {
            throw new RuntimeException(String.valueOf(R.string.exception_FAILED_TO_OPEN_DEVICE));
        }

        myText.append("" + getString(R.string.msg_wait_for_sensor_data) + "\n");
        Log.d(LOGTAG, "displaying " + getString(R.string.msg_wait_for_sensor_data));

        RtlSdr.open(arguments, connection.getFileDescriptor(), UsbHelper.properDeviceName(device.getDeviceName()));

        myText.append("" + getString(R.string.msg_rcving_sensor_data) + "\n");
        Log.d(LOGTAG, "displaying " + getString(R.string.msg_rcving_sensor_data));
        Log.i(LOGTAG, "onActivityResult: RTL-SDR Data Capture Completed.");
        processRtlsdrData();

        running = false;
        updateActionBarAnimation();
        Log.i(LOGTAG, "Exit run");
    }

    private void processRtlsdrData() {
        Log.i(LOGTAG, "Enter processRtlsdrData");
        convertAndGetPkt();
        // file.delete();
        if (pktFound() == 1) {
            String hxAddr = getHexAddr();
            Log.i(LOGTAG, "hxAddr is >" + hxAddr + "<");
            myText.append(getString(R.string.sensor_id) + " " + hxAddr + "\n");
            Log.d(LOGTAG, "displaying " + getString(R.string.sensor_id) + " " + hxAddr);

            long tmpC = getTempC();
            Log.i(LOGTAG, "tmpC is >" + (double) (tmpC) + "<");
            myText.append(getString(R.string.temp) + " " + ((double) ((int) (tmpC * 10))) / 10 + getString(R.string.celsius) + "\n");
            Log.d(LOGTAG, "displaying " + getString(R.string.temp) + " " + ((double) ((int) (tmpC * 10))) / 10 + getString(R.string.celsius));

            double press_psi = getPsi();
            Log.i(LOGTAG, "press_psi is >" + press_psi + "<");
            myText.append(getString(R.string.pressure) + " " + ((double) ((int) (press_psi * 100))) / 100 + getString(R.string.psi) + "\n");
            Log.d(LOGTAG, "displaying " + getString(R.string.pressure) + " " + ((double) ((int) (press_psi * 100))) / 100 + getString(R.string.psi));

            myText.append(getString(R.string.msg_sending_data) + "\n");
            Log.d(LOGTAG, "displaying " + getString(R.string.msg_sending_data));

            // Instantiate the RequestQueue.
            RequestQueue queue = Volley.newRequestQueue(this);
            String url = getFullUrl();
            Log.i(LOGTAG, "url is >" + url + "<");
            // Request a string response from the provided URL.
            StringRequest stringRequest = new StringRequest(StringRequest.Method.GET, url,
                    new Response.Listener<String>() {
                        @Override
                        public void onResponse(String response) {
                            myText.append(getString(R.string.msg_data_rcvd_ok) + "\n");
                            Log.d(LOGTAG, "displaying " + getString(R.string.msg_data_rcvd_ok));
                        }
                    }, new Response.ErrorListener() {
                @Override
                public void onErrorResponse(VolleyError error) {
                    myText.append("" + getString(R.string.msg_data_not_rcvd_ok) + "\n");
                    Log.d(LOGTAG, "displaying " + getString(R.string.msg_data_not_rcvd_ok));
                }
            });
            // Add the request to the RequestQueue.
            queue.add(stringRequest);
        } else {
            myText.append("" + getString(R.string.msg_valid_pkt_not_found) + "\n");
            Log.d(LOGTAG, "displaying " + getString(R.string.msg_valid_pkt_not_found));
        }
        Log.i(LOGTAG, "Exit processRtlsdrData");
    }



    /**
     * Announce there is an error. The id of the error could be get with RtlTcpExceptionId
     * from the parent activity.
     */
    public void finishWithError(int id, Integer second_id, String msg) {
        final Intent data = new Intent();
        data.putExtra("com.fleetcents.generic_tpms.RtlTcpExceptionId", id);

        if (second_id != null) data.putExtra("detailed_exception_code", second_id);
        if (msg != null) data.putExtra("detailed_exception_message", msg);

        if (getParent() == null) {
            setResult(RESULT_CANCELED, data);
        } else {
            getParent().setResult(RESULT_CANCELED, data);
        }
        finish();
    }

    public void finishWithError(final Exception e) {
        if (e == null) {
            finishWithError();
            return;
        }
        if ((e instanceof RtlSdrStartException))
            finishWithError(((RtlSdrStartException) e).getReason());
        else if (e instanceof RtlSdrException) {
            final RtlSdrException rtlexception = (RtlSdrException) e;
            finishWithError(rtlexception.getReason(), rtlexception.id, rtlexception.getMessage());
        } else
            finishWithError();
    }

    public void finishWithError(final RtlSdrStartException.err_info info) {
        finishWithError(info, null, null);
    }

    public void finishWithError(final RtlSdrStartException.err_info info, Integer second_id, String msg) {
        if (info != null)
            finishWithError(info.ordinal(), second_id, msg);
        else
            finishWithError(second_id, msg);
    }

    public void finishWithError() {
        finishWithError(null, null);
    }

    public void finishWithError(Integer second_id, String msg) {
        finishWithError(-1, second_id, msg);
    }

    private void finishWithSuccess() {
        final Intent data = new Intent();

        if (getParent() == null) {
            setResult(RESULT_OK, data);
        } else {
            getParent().setResult(RESULT_OK, data);
        }
        finish();
    }

}