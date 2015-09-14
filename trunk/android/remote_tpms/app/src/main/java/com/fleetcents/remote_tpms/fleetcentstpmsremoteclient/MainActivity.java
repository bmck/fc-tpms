package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.hardware.usb.UsbManager;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v4.content.ContextCompat;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.TextView;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

public class MainActivity extends Activity {

    private static final String LOGTAG = "MainActivity";
    private static final String TAG = "FleetCents";
    private static final String TMPFCBINFN = "fc.bin";
    public static PendingIntent permissionIntent;
    public static Intent intent;
    public static MainActivity activity = null;

    static {
        try {
            Log.i(LOGTAG, "Loading libraries");
            System.loadLibrary("crystax");
            System.loadLibrary("rtlsdr");
            Log.i(LOGTAG, "Successfully loaded native libraries");
        } catch (Throwable t) {
            Log.w(LOGTAG, "Failed to load native libraries");
        }
    }

    public boolean running = false;
    private MenuItem mi_startStop = null;
    private TextView myText = null;
    private boolean testing = false;
    private String arguments = "";
    private String uspfs_path = null;
    private int fd = -1;
    private UsbManager mUsbManager = null;

    public String cacheDir(String fn) {
        // emulator expects dir at /storage/sdcard/Android/data/com.fleetcents.remote_tpms.fleetcentstpmsremoteclient/cache
        File[] dirs = ContextCompat.getExternalFilesDirs(this, "rtlsdr");

        File fdir = (dirs.length >= 1) ? dirs[dirs.length - 1] : getFilesDir();
        String retVal = String.valueOf(fdir);

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
        MainActivity.activity = this;
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // The filter's action is BROADCAST_ACTION & Add a data filter for the HTTP scheme
        IntentFilter mStatusIntentFilter = new IntentFilter(RtlSdrFeedbackTypes.BROADCAST_ACTION);
        // Instantiates a new FeedbackReceiver, & register it and its intent filters
        FeedbackReceiver mFeedbackReceiver = new FeedbackReceiver();
        LocalBroadcastManager.getInstance(this).registerReceiver(mFeedbackReceiver, mStatusIntentFilter);

        Log.i(LOGTAG, "whats wrong with following line?");
        setContentView(R.layout.main_layout);

        myText = (TextView) findViewById(R.id.main_messages);
        myText.setInputType(0x00020001);
        myText.setText(getString(R.string.hello_world) + "\n");

        running = false;

        Log.i(LOGTAG, "Exit onCreate");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.i(LOGTAG, "Enter onCreateOptionsMenu");

        getMenuInflater().inflate(R.menu.main_menu, menu);

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
            case R.id.action_settings:
                enterSettings(findViewById(R.id.action_settings));
                return true;

            case R.id.action_startstop:
                if (!running) {
                    running = true;
                    updateActionBarAnimation();

                    File file = new File(cacheDir(TMPFCBINFN), TMPFCBINFN);
                    if (file == null) {
                        errorModalBox(getString(R.string.exception_RTLSDR_FILE_NOT_SAVED));
                        return false;
                    }
                    int sample_rate = 2048000;
                    SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);
                    double secs = Double.parseDouble(sharedPrefs.getString("basestation_secssent", "1.0"));
                    String base = sharedPrefs.getString("basestation_hostname", "127.0.0.1");
                    String tempDisplay = sharedPrefs.getString("units_temp", getString(R.string.celsius));
                    String pressDisplay = sharedPrefs.getString("units_pressure", getString(R.string.psi));
                    // String arguments = "-f 314980000 -s " + sample_rate + " -n " + Math.round(secs * sample_rate) + " -t " + (testing ? "1" : "0") + " " + file + "";
                    // Log.i(LOGTAG, "arguments = >" + arguments + "<");
                    String arguments = "?f=314980000&s=" + sample_rate + "&n=" + Math.round(secs * sample_rate) +
                            "&base=" + base + "&c=" + (tempDisplay == getString(R.string.celsius) ? 1 : 0) +
                            "&p=" + (pressDisplay == getString(R.string.psi) ? 1 : 0) +
                            "&t=" + (testing ? "1" : "0") + "&fn=" + file;

                    Intent mServiceIntent = new Intent(this, SdrFtpService.class);
                    mServiceIntent.setData(Uri.parse("rtlsdr://" + arguments));
                    startService(mServiceIntent);
                }
                Log.i(LOGTAG, "Exit onOptionsItemSelected");
                return true;
            default:
                Log.i(LOGTAG, "Exit onOptionsItemSelected");
                return super.onOptionsItemSelected(item);
        }
    }

    public void updateActionBarAnimation() {
        Log.i(LOGTAG, "Enter updateActionBarAnimation");
        // Set icon of the start/stop button according to the state:
        if (mi_startStop != null) {
            if (running) {
                LayoutInflater inflater = (LayoutInflater) this.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                ImageView imageView = (ImageView) inflater.inflate(R.layout.start_stop_layout, null);
                Animation fade = AnimationUtils.loadAnimation(this, R.anim.fade);
                imageView.startAnimation(fade);
                mi_startStop.setActionView(R.layout.start_stop_layout);
            } else {
                mi_startStop.setActionView(null);
            }
        }
        Log.i(LOGTAG, "Exit updateActionBarAnimation");
    }

    public void enterSettings(View view) {
        Intent intent = new Intent(this, SettingsActivity.class);
        startActivity(intent);
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

    public void errorModalBox(final String str) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Log.d(LOGTAG, ">>>> displaying modal box -- " + str + "\n");

                AlertDialog.Builder builder = new AlertDialog.Builder(activity);
                builder.setMessage(str).setTitle(R.string.error).setNeutralButton(
                        getString(R.string.btn_ok), new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                // User clicked button
                                dialog.dismiss();
                                displayMessage("\n\n\n\n------------------\n\n\n\n");
                                running = false;
                                updateActionBarAnimation();
                            }
                        });
                builder.show();
            }
        });
    }

    /**
     * Announce there is an error. The id of the error could be get with RtlTcpExceptionId
     * from the parent
     */

    public void finishWithError(int id, Integer second_id, String msg) {
        final Intent data = new Intent();
        data.putExtra("com.fleetcents.remote_tpms.fleetcentstpmsremoteclient.RtlTcpExceptionId", id);

        if (second_id != null) data.putExtra("detailed_exception_code", second_id);
        if (msg != null) data.putExtra("detailed_exception_message", msg);

        if (getParent() == null) {
            setResult(RESULT_CANCELED, data);
        } else {
            getParent().setResult(RESULT_CANCELED, data);
        }

        activity.errorModalBox(RtlSdrException.translateToString(second_id));

        activity.finish();
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
            finishWithError(RtlSdrStartException.err_info.unknown_error, RtlSdr.LIBUSB_ERROR_OTHER, e.getMessage());
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

    // Broadcast receiver for receiving status updates from the IntentService
    private class FeedbackReceiver extends BroadcastReceiver {
        // Prevents instantiation
        private FeedbackReceiver() {
        }

        // Called when the BroadcastReceiver gets an Intent it's registered to receive
        @Override
        public void onReceive(Context context, Intent intent) {
            String str;

            str = intent.getStringExtra(RtlSdrFeedbackTypes.EXTENDED_DATA_STATUS);
            if (str != null) {
                Log.i(LOGTAG, "received displayMessage, str = >" + str + "<");
                displayMessage(str);
            } else {
                str = intent.getStringExtra(RtlSdrFeedbackTypes.EXTENDED_DATA_COMPLETEOK);
                if (str != null) {
                    running = false;
                    updateActionBarAnimation();
                } else {
                    str = intent.getStringExtra(RtlSdrFeedbackTypes.EXTENDED_DATA_FAILURE);
                    if (str == null)
                        str = getString(R.string.exception_UNKNOWN);
                    Log.i(LOGTAG, "received errorModalBox, status = >" + str + "<");
                    errorModalBox(str);
                }
            }
        }
    }
}