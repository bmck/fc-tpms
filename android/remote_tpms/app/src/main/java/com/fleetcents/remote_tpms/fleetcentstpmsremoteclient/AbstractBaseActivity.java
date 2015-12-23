package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.PendingIntent;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.SharedPreferences;
import android.hardware.usb.UsbManager;
import android.preference.PreferenceManager;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;

/**
 * Created by bmck on 12/1/15.
 */
public class AbstractBaseActivity extends Activity {

    private static final String LOGTAG = "AbstractBaseActivity";
    SharedPreferences sharedPrefs = null;
    protected static boolean credsProvided = false;
    static String apiToken = "";
    protected boolean serviceCallHandled = false;

    public static String apiToken() { return apiToken; }
    public static boolean apiTokenAvailable() { return ((apiToken != null) && (apiToken.length() > 0)); }
    public static void setApiToken(String newToken) { apiToken = newToken; }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main_menu, menu);
        return true;
    }

    protected void enterSettings(View view) {
        Intent intent = new Intent(this, SettingsActivity.class);
        startActivity(intent);
    }

    SharedPreferences.OnSharedPreferenceChangeListener listener = null;

    @Override
    protected void onResume() {
        super.onResume();
        if (sharedPrefs == null) {
            sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);
        }

        credsProvided = ((sharedPrefs.getString("userid", "").length() > 0) &&
                (sharedPrefs.getString("password", "").length() > 0));

        listener = new SharedPreferences.OnSharedPreferenceChangeListener() {
            public void onSharedPreferenceChanged(SharedPreferences prefs, String key) {
                // listener implementation
                if ((key.equals("userid")) || (key.equals("password"))) {
                    // Set summary to be the user-description for the selected value
                    credsProvided = (sharedPrefs.getString("userid", "").length() > 0) &&
                            (sharedPrefs.getString("password", "").length() > 0);
                }
            }
        };
        sharedPrefs.registerOnSharedPreferenceChangeListener(listener);
    }

    @Override
    protected void onPause() {
        super.onPause();
        if ((sharedPrefs != null) && (listener != null)) {
            sharedPrefs.unregisterOnSharedPreferenceChangeListener(listener);
        }
    }    protected void enterLogView(View view) {
        Intent intent = new Intent(this, LogActivity.class);
        startActivity(intent);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_settings:
                enterSettings(findViewById(R.id.action_settings));
                return true;
            case R.id.action_startstop:
                enterLogView(findViewById(R.id.action_startstop));
                return true;
            default:
                Log.i(LOGTAG, "Exit onOptionsItemSelected");
                return super.onOptionsItemSelected(item);
        }
    }

    protected void errorModalBox(final String str) {
        final AbstractBaseActivity activity = this;

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                AlertDialog.Builder builder = new AlertDialog.Builder(activity);
                builder.setMessage(str).setTitle(R.string.error).setNeutralButton(
                        getString(R.string.btn_ok), new DialogInterface.OnClickListener() {
                            public void onClick(DialogInterface dialog, int id) {
                                // User clicked button
                                dialog.dismiss();
                            }
                        });
                builder.show();
            }
        });
    }
}
