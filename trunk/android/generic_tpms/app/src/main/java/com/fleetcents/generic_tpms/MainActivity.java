package com.fleetcents.generic_tpms;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Message;
import android.support.v4.content.ContextCompat;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageView;
import android.widget.TextView;
import android.app.PendingIntent;

import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.fleetcents.generic_tpms.core.RtlSdr;
import com.fleetcents.generic_tpms.core.RtlSdrException;
import com.fleetcents.generic_tpms.core.RtlSdrStartException;
import com.fleetcents.generic_tpms.core.UsbHelper;

public class MainActivity extends Activity {

    private static final String LOGTAG = "MainActivity";
    private static final String TAG = "FleetCents";
    private static final String TMPFCBINFN = "fc.bin";

    private MenuItem mi_startStop = null;
    private TextView myText = null;
    private boolean running = false;
    private boolean testing = false;

   public static native void setCacheDir(String str);

    public static native int pktFound();

//    public static native void convertAndGetPkt();

    public static native double getKpa();

    public static native double getPsi();

    public static native long getTempF();

    public static native long getTempC();

    public static native long getDecAddr();

    public static native String getHexAddr();

    public static native String getFullUrl();

    private String arguments = "";
    private String uspfs_path = null;
    private int fd = -1;

    private UsbManager mUsbManager = null;
    public static PendingIntent permissionIntent;
    public static Intent intent;

    public MainActivity activity = this;

    private class WorkerThread extends Thread {
        @Override
        public void run() {
            synchronized (this) {
                Log.i(LOGTAG, "A @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()) + "\n");
                displayMessage(getString(R.string.msg_send_activation));

                File file = new File(cacheDir(TMPFCBINFN), TMPFCBINFN);
                arguments = "-f 315000000 -s 2048000 -n 4000000 -t " + (testing ? "1" : "0") + " -S" + " " + file + "";
                Log.i(LOGTAG, "arguments = >" + arguments + "<");

                if (!testing) {
                    final UsbDevice device = UsbHelper.findDevice(activity);
                    final UsbDeviceConnection connection = UsbHelper.openDevice(activity, device);
                    if (connection == null) {
                        throw new RuntimeException(String.valueOf(R.string.exception_FAILED_TO_OPEN_DEVICE));
                    }

                    displayMessage(getString(R.string.msg_wait_for_sensor_data));

                    Log.d(LOGTAG, "B @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                    RtlSdr.open(arguments, connection.getFileDescriptor(), UsbHelper.properDeviceName(device.getDeviceName()));
                    Log.d(LOGTAG, "C @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                }
                else {
                    displayMessage(getString(R.string.msg_wait_for_sensor_data));

                    Log.d(LOGTAG, "B @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                    RtlSdr.open(arguments, -1, null);
                    Log.d(LOGTAG, "C @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));

                }
                displayMessage(getString(R.string.msg_rcving_sensor_data));
                Log.i(LOGTAG, "onActivityResult: RTL-SDR Data Capture Completed.");

                if (pktFound() == 1) {
                    String hxAddr = getHexAddr();
                    Log.i(LOGTAG, "hxAddr is >" + hxAddr + "<");
                    displayMessage(getString(R.string.sensor_id) + " " + hxAddr);

                    long tmpC = getTempC();
                    Log.i(LOGTAG, "tmpC is >" + (double) (tmpC) + "<");
                    displayMessage(getString(R.string.temp) + " " + ((double) ((int) (tmpC * 10))) / 10 + getString(R.string.celsius));

                    double press_psi = getPsi();
                    Log.i(LOGTAG, "press_psi is >" + press_psi + "<");
                    displayMessage(getString(R.string.pressure) + " " + ((double) ((int) (press_psi * 100))) / 100 + getString(R.string.psi));

                    displayMessage(getString(R.string.msg_sending_data));

                    // Instantiate the RequestQueue.
                    RequestQueue queue = Volley.newRequestQueue(activity);
                    String url = getFullUrl();
                    Log.i(LOGTAG, "url is >" + url + "<");
                    // Request a string response from the provided URL.
                    StringRequest stringRequest = new StringRequest(StringRequest.Method.GET, url,
                            new Response.Listener<String>() {
                                @Override
                                public void onResponse(String response) {
                                    displayMessage(getString(R.string.msg_data_rcvd_ok) + "\n\n");
                                }
                            }, new Response.ErrorListener() {
                        @Override
                        public void onErrorResponse(VolleyError error) {
                            displayMessage(getString(R.string.msg_data_not_rcvd_ok) + "\n\n");
                        }
                    });
                    // Add the request to the RequestQueue.
                    queue.add(stringRequest);
                } else {
                    displayMessage(getString(R.string.msg_valid_pkt_not_found) + "\n\n");
                }

                Log.i(LOGTAG, "Exit postProcessRtlsdrData");
                Log.i(LOGTAG, "F @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()) + "\n");

                running = false;
                updateActionBarAnimation();
                Log.i(LOGTAG, "G @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()) + "\n");

                this.notify();
            }
        }
    };

    static {
        try {
            Log.i(LOGTAG, "Loading libraries");
            System.loadLibrary("crystax");
            System.loadLibrary("liquid");
            System.loadLibrary("fleetcents");
            System.loadLibrary("rtlsdr");
            Log.i(LOGTAG, "Successfully loaded native libraries");
        } catch (Throwable t) {
            Log.w(LOGTAG, "Failed to load native libraries");
        }
    }

   public String cacheDir(String fn) {
       // emulator expects dir at /storage/sdcard/Android/data/com.fleetcents.generic_tpms/cache
       File[] dirs = ContextCompat.getExternalFilesDirs(this, "rtlsdr");
       File fdir = dirs[dirs.length - 1];
       String retVal = String.valueOf(fdir);
       setCacheDir(retVal);
       if (!testing) {
           File f = new File(fdir, fn);
           f.mkdirs();
           f.delete();
       }
       return retVal;
   }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(LOGTAG, "Enter onCreate");
        super.onCreate(savedInstanceState);
        Log.i(LOGTAG, "whats wrong with following line?");
        setContentView(R.layout.activity_main);

        myText = (TextView) findViewById(R.id.main_messages);
        myText.setInputType(0x00020001);
        myText.setText(getString(R.string.hello_world) + "\n");

        running = false;

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

        permissionIntent = PendingIntent.getBroadcast(this, 0, new Intent("com.android.example.USB_PERMISSION"), 0);
        mUsbManager = (UsbManager) this.getSystemService(Context.USB_SERVICE);

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
                    running = true;
                    updateActionBarAnimation();
                    WorkerThread t = new WorkerThread();
                    synchronized (t) {
                        t.start();
                        try {
                            t.wait();
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
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

    private void displayMessage(final String str) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                myText.append(str + "\n");
                Log.d(LOGTAG, "displayed >" + str + "<?\n");
            }
        });
        Log.d(LOGTAG, "display >" + str + "<?\n");
    }


    /**
     * Announce there is an error. The id of the error could be get with RtlTcpExceptionId
     * from the parent
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