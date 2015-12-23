package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AbstractBaseActivity {

    private static final String LOGTAG = "MainActivity";

    protected boolean init() {
        if (!isNetworkAvailable(this)) {
            errorModalBox("Internet connectivity not found.  Please correct and restart app.");
            return false;
        }
        return true;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        init();

        setContentView(R.layout.activity_main);
    }

    @Override
    protected void onResume() {
        super.onResume();

        TextView mMainTitle = (TextView) findViewById(R.id.main_title);
        mMainTitle.setInputType(0x00020001);
        mMainTitle.setTextSize(16);
        Log.i(LOGTAG, "" + getString(R.string.app_name) + " - version " + getString(R.string.version) + "\n");
        mMainTitle.setText(getString(R.string.app_name) + " - version " + getString(R.string.version) + "\n");

        View mLoginView = findViewById(R.id.set_user_creds);
        View mDisplayFleet = findViewById(R.id.display_fleet);
        View mLogoutView = findViewById(R.id.user_logout);

        mLoginView.setVisibility(apiTokenAvailable() ? View.GONE : View.VISIBLE);
        mDisplayFleet.setVisibility(apiTokenAvailable() ? View.VISIBLE : View.GONE);
        mLogoutView.setVisibility(apiTokenAvailable() ? View.VISIBLE : View.GONE);
    }

    public void set_user_creds(View v) {
        Log.i(LOGTAG, "click set_user_creds");
        Intent i = new Intent(getApplicationContext(), LoginActivity.class);
        startActivity(i);
    }

    public void display_fleet(View v) {
        Log.i(LOGTAG, "click display_fleet");
        Intent i = new Intent(getApplicationContext(), DisplayFleetActivity.class);
        startActivity(i);
    }

    public void collect_tire_sample(View v) {
        Log.i(LOGTAG, "click collect_tire_sample");
        Intent i = new Intent(getApplicationContext(), LogActivity.class);
        startActivity(i);
    }

    public void user_logout(View v) {
        Log.i(LOGTAG, "click user_logout");

        class UserLogoutTask extends AsyncTask<Void, Void, Boolean> {

            UserLogoutTask() {
            }

            @Override
            protected Boolean doInBackground(Void... params) {
                FcQueryService svc = new FcQueryService();
                return svc.logoutOfServer();
            }
        }

        credsProvided = false;
        (new UserLogoutTask()).execute((Void) null);
        recreate();
    }

    //check for internet connection
    private boolean isNetworkAvailable(Context context) {
        ConnectivityManager connectivity = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
        if (connectivity != null) {
            NetworkInfo[] info = connectivity.getAllNetworkInfo();
            if (info != null) {
                for (int i = 0; i < info.length; i++) {
                    Log.v(LOGTAG, String.valueOf(i));
                    if (info[i].getState() == NetworkInfo.State.CONNECTED) {
                        Log.v(LOGTAG, "connected!");
                        return true;
                    }
                }
            }
        }
        return false;
    }
}
