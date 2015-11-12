package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.ActionBar;
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
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.LinearInterpolator;
import android.widget.ImageView;
import android.widget.ScrollView;
import android.widget.TextView;

import org.acra.ACRA;

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
            System.loadLibrary("liquid");
            System.loadLibrary("fleetcents");
            Log.i(LOGTAG, "Successfully loaded native libraries");
        } catch (Throwable t) {
            Log.w(LOGTAG, "Failed to load native libraries");
        }
    }

    public boolean running = false;
    public boolean abort_requested = false;
    private MenuItem mi_startStop = null;
    private TextView myText = null;
    private ScrollView scroll = null;
    private boolean testing = false;
    private String arguments = "";
    private String uspfs_path = null;
    private int fd = -1;
    private UsbManager mUsbManager = null;
    private boolean detailed_logging = false;

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

        scroll = (ScrollView) this.findViewById(R.id.scroll);

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

        switch (item.getItemId()) {
            case R.id.action_settings:
                enterSettings(findViewById(R.id.action_settings));
                return true;

            case R.id.action_startstop:
                // TODO: Handle 3 cases -- stopped (show play, press to start), started (show pause or stop, press to request abort), abort requested (show flashing pause or stop, press to ignore)
                // Stopped, now starting
                if (running == false) {
                    SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);
                    detailed_logging = sharedPrefs.getBoolean("detailed_messages", false);

                    running = true;
                    abort_requested = false;
                    updateActionBarAnimation();

                    File file = new File(cacheDir(TMPFCBINFN), TMPFCBINFN);
                    log_it("i", LOGTAG, "Temporary logfile: " + String.valueOf(file));
                    if (file == null) {
                        errorModalBox(getString(R.string.exception_RTLSDR_FILE_NOT_SAVED));
                        return false;
                    }
                    int sample_rate = 2048000;
                    double secs = Double.parseDouble(sharedPrefs.getString("basestation_secssent", "1.0"));
                    String loc = sharedPrefs.getString("basestation_local_remote", getString(R.string.local));
                    int proc_local = (loc.equals(getString(R.string.local)) ? 1 : 0);

                    String base = sharedPrefs.getString("basestation_remote_hostname", "127.0.0.1");

                    String tempDisplay = sharedPrefs.getString("units_temp", getString(R.string.celsius));
                    log_it("i", LOGTAG, "tempDisplay: " + tempDisplay);
                    int temp_var = tempDisplay.equals(R.string.celsius) ? 1 : 0;
                    log_it("i", LOGTAG, "temp_var: " + temp_var);

                    String pressDisplay = sharedPrefs.getString("units_pressure", getString(R.string.psi));
                    log_it("i", LOGTAG, "pressDisplay: " + pressDisplay);
                    int temp_press = pressDisplay.equals(R.string.psi) ? 1 : 0;
                    log_it("i", LOGTAG, "temp_press: " + temp_press);

                    boolean gain_adjust = sharedPrefs.getBoolean("gain_adjust", true);

                    String arguments = "?f=314980000&s=" + sample_rate + "&n=" + Math.round(secs * sample_rate) +
                            "&base=" + base + "&c=" + temp_var +
                            "&p=" + temp_press + "&q=" + (gain_adjust == true ? "1" : "0") +
                            "&l=" + proc_local +
                            "&t=" + (testing ? "1" : "0") + "&fn=" + file;

                    Intent mServiceIntent = new Intent(this, SdrFtpService.class);
                    mServiceIntent.setData(Uri.parse("rtlsdr://" + arguments));
                    startService(mServiceIntent);
                }
                // Running, requested stop
                else if (abort_requested == false) {
                  abort_requested = true;
                    updateActionBarAnimation();
                }
                // Stop requested, now stopping
                else {
                    running = false;
                    abort_requested = false;
                    updateActionBarAnimation();
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
            if (abort_requested) {
                // Running, requested stop
                mi_startStop.setIcon(R.drawable.ic_pause_filled);

                LayoutInflater inflater = (LayoutInflater) activity.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                ImageView iv = (ImageView) inflater.inflate(R.layout.start_stop_layout, null);
                Animation fader = AnimationUtils.loadAnimation(activity, R.anim.fade);
                fader.setRepeatCount(Animation.INFINITE);
                iv.startAnimation(fader);
                mi_startStop.setActionView(iv);
            }
            else if (running) {
                // Stopped, now starting
                if (mi_startStop.getActionView() != null)  mi_startStop.getActionView().clearAnimation();
                mi_startStop.setActionView(null);
                mi_startStop.setIcon(R.drawable.ic_action_pause);
            } else {
                // Stop requested, now stopping
                if (mi_startStop.getActionView() != null) mi_startStop.getActionView().clearAnimation();
                mi_startStop.setActionView(null);
                mi_startStop.setIcon(R.drawable.ic_action_play);
            }
        }
        Log.i(LOGTAG, "Exit updateActionBarAnimation");
    }

    public void log_it(String log_lvl, String tag, String str) {
        if (!("idwev".contains(log_lvl))) log_lvl = "i";
        if (detailed_logging) displayMessage(str);
        if (log_lvl == "i") Log.i(tag, str);
        else if (log_lvl == "d") Log.d(tag, str);
        else if (log_lvl == "w") Log.w(tag, str);
        else if (log_lvl == "e") Log.e(tag, str);
        else Log.v(tag, str);
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
                scroll.scrollTo(0, scroll.getBottom());
            }
        });
    }

    public void errorModalBox(final String str) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AlertDialog.Builder builder = new AlertDialog.Builder(activity);
                builder.setMessage(str).setTitle(R.string.error).setNeutralButton(
                        getString(R.string.btn_ok), new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                // User clicked button
                                dialog.dismiss();
                                displayMessage("\n\n\n\n------------------\n\n\n\n");
                                running = false;
                                abort_requested = false;
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

        if (mi_startStop.getActionView() != null) {
            mi_startStop.getActionView().clearAnimation();
            mi_startStop.setActionView(null);
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
        if (mi_startStop.getActionView() != null) {
            mi_startStop.getActionView().clearAnimation();
            mi_startStop.setActionView(null);
        }
        finish();
    }

    // Broadcast receiver for receiving status updates from the IntentService
    private class FeedbackReceiver extends BroadcastReceiver {
        private FeedbackReceiver() {
        }

        // Called when the BroadcastReceiver gets an Intent it's registered to receive
        @Override
        public void onReceive(Context context, Intent intent) {
            String str;

            str = intent.getStringExtra(RtlSdrFeedbackTypes.EXTENDED_DATA_STATUS);
            if (str != null) {
                displayMessage(str);
            } else {
                if (mi_startStop.getActionView() != null) {
                    mi_startStop.getActionView().clearAnimation();
                    mi_startStop.setActionView(null);
                }

                str = intent.getStringExtra(RtlSdrFeedbackTypes.EXTENDED_DATA_COMPLETEOK);
                if (str != null) {
                    running = false;
                    updateActionBarAnimation();
                } else {
                    str = intent.getStringExtra(RtlSdrFeedbackTypes.EXTENDED_DATA_FAILURE);
                    if (str == null)
                        str = getString(R.string.exception_UNKNOWN);
                    errorModalBox(str);
                }
            }
        }
    }
}