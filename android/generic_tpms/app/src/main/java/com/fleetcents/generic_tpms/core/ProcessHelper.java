
package com.fleetcents.generic_tpms.core;

import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.util.Log;

import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import com.fleetcents.generic_tpms.R;
import com.fleetcents.generic_tpms.MainActivity;

import java.io.File;

/**
 * Created by bmck on 7/1/15.
 */
public class ProcessHelper {

    private static final String LOGTAG = "ProcessHelper";
    private static final String TMPFCBINFN = "fc.bin";
    private MainActivity activity;

    public static native int pktFound();

    public static native void convertAndGetPkt();

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

    public ProcessHelper(MainActivity act) {
        activity = act;
        activity.permissionIntent = PendingIntent.getBroadcast(act, 0, new Intent("com.android.example.USB_PERMISSION"), 0);

        mUsbManager = (UsbManager) act.getSystemService(Context.USB_SERVICE);
        Log.i(LOGTAG, "USB Manager found.");
    }

    public void run(boolean testing) {
        if (!testing) {
            displayMessage(getString(R.string.msg_send_activation));
            Log.d(LOGTAG, "displaying " + getString(R.string.msg_send_activation));

            File file = new File(activity.cacheDir(TMPFCBINFN), TMPFCBINFN);
            arguments = "-f 315000000 -s 2048000 -n 4000000 -S " + file + "";
            Log.i(LOGTAG, "arguments = >" + arguments + "<");

            UsbDevice device = UsbHelper.findDevice(activity);

            final UsbDeviceConnection connection = UsbHelper.openDevice(activity, device);
            if (connection == null) {
                throw new RuntimeException(String.valueOf(R.string.exception_FAILED_TO_OPEN_DEVICE));
            }

            displayMessage(getString(R.string.msg_wait_for_sensor_data));
            Log.d(LOGTAG, "displaying " + getString(R.string.msg_wait_for_sensor_data));

            RtlSdr.open(arguments, connection.getFileDescriptor(), UsbHelper.properDeviceName(device.getDeviceName()));
        }

        displayMessage(getString(R.string.msg_rcving_sensor_data));
        Log.i(LOGTAG, "onActivityResult: RTL-SDR Data Capture Completed.");

        convertAndGetPkt();
        // file.delete();
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
                            displayMessage(getString(R.string.msg_data_rcvd_ok));
                        }
                    }, new Response.ErrorListener() {
                @Override
                public void onErrorResponse(VolleyError error) {
                    displayMessage(getString(R.string.msg_data_not_rcvd_ok));
                }
            });
            // Add the request to the RequestQueue.
            queue.add(stringRequest);
        } else {
            displayMessage(getString(R.string.msg_valid_pkt_not_found));
        }
        Log.i(LOGTAG, "Exit postProcessRtlsdrData");
    }

    private String getString(int resId) {
        return activity.getString(resId);
    }

    private void displayMessage(String str) {
        activity.displayMessage(str);
    }

}
