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

import com.fleetcents.generic_tpms.core.FcTextView;
import com.fleetcents.generic_tpms.core.RtlSdr;
import com.fleetcents.generic_tpms.core.RtlSdrException;
import com.fleetcents.generic_tpms.core.RtlSdrStartException;
import com.fleetcents.generic_tpms.core.UsbHelper;
import com.fleetcents.generic_tpms.core.ProcessHelper;
import com.fleetcents.generic_tpms.core.UsbHelper.STATUS;

public class MainActivity extends Activity implements Runnable {

    // private static final int RTL2832U_RESULT_CODE = 1234;  // arbitrary value, used when sending intent to RTL2832U

    private static final String LOGTAG = "MainActivity";
    public static final String TAG = "FleetCents";
    private MenuItem mi_startStop = null;
    private boolean running = false;
    private boolean testing = false;

    public static PendingIntent permissionIntent;
    public static Intent intent;

    public static native void setCacheDir(String str);

    private ProcessHelper pHelper = null;
    private FcTextView myText = null;

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

    public String cacheDir(String fn) {
        // emulator expects dir at /storage/sdcard/Android/data/com.fleetcents.generic_tpms/cache
        Log.i(LOGTAG, "Enter cacheDir");
        File[] dirs = ContextCompat.getExternalFilesDirs(this, "rtlsdr");
        File fdir = dirs[dirs.length-1];
        String retVal = String.valueOf(fdir);
        Log.i(LOGTAG, "cached dir = " + fdir.getPath());
        setCacheDir(retVal);
        File f = new File(fdir, fn);
        f.mkdirs();
        f.delete();

        Log.i(LOGTAG, "Exit cacheDir");
        return retVal;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(LOGTAG, "Enter onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        myText = new FcTextView(this);
        myText.setInputType(0x00020001); // multi-line text box
        LinearLayout linearLayout = (LinearLayout) findViewById(R.id.main_activity);
        linearLayout.addView(myText, new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));

        running = false;
        pHelper = new ProcessHelper(this);

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
                    running = true;
                    updateActionBarAnimation();
                    run();
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
        pHelper.run(testing);
        running = false;
        updateActionBarAnimation();
        Log.i(LOGTAG, "Exit run");
    }

    public void displayMessage(final String str) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                myText.append(str + "\n");
                myText.invalidate();
            }
        });
        Log.d(LOGTAG, "displaying >" + str + "<\n");
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