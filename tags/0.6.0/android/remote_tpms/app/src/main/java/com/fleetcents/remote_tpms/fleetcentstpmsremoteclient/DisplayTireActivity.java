package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.widget.RelativeLayout;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;

import org.acra.ACRA;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class DisplayTireActivity extends AbstractBaseActivity {

    private static final String LOGTAG = "DisplayTireActivity";
    protected String tireSampleUri = "/api/v1/tire_samples.json";
    protected String tireSampleParms = "tire_id=";

    private int tireId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_tire);
    }

    private BroadcastReceiver bReceiver = new BroadcastReceiver(){
        @Override
        public void onReceive(Context context, Intent intent) {
            //put here whaterver you want your activity to do with the intent received
            String response = intent.getStringExtra("response");
            try {
                JSONObject jsonObj = new JSONObject(response);
                handleServerResponse(jsonObj);
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    };

    protected void onResume() {
        super.onResume();
        String prms = "meth=GET";

        Bundle extras = getIntent().getExtras();
        if (extras != null) {
            tireId = extras.getInt("EXTRA_TIRE_ID");
            prms = prms + "&" + tireSampleParms + tireId;
        }
        String arguments = tireSampleUri + "?" + prms;
        LocalBroadcastManager.getInstance(this).registerReceiver(bReceiver, new IntentFilter(arguments));

        RelativeLayout layout = (RelativeLayout) findViewById(R.id.display_fleet_layout);

        TextView tire_instructs = (TextView) findViewById(R.id.tirestamp_dirs);
        tire_instructs.setInputType(0x00020001);

        if (credsProvided) {
            tire_instructs.setTextSize(16);
            tire_instructs.setText(getString(R.string.tire_samples_instructs));

//            callToServer(tireSampleUri, prms, "GET");
            Intent mServiceIntent = new Intent(this, FcQueryService.class);
            mServiceIntent.setData(Uri.parse("fc://" + arguments));
            startService(mServiceIntent);
        } else {
            tire_instructs.setTextSize(24);
            tire_instructs.setText(getString(R.string.update_creds));
        }
    }

    protected void handleServerResponse(JSONObject jsonObj) {
        Log.i(LOGTAG, "Calling " + LOGTAG + "handleServerResponse\n");

        // parse response, and add buttons to UI panel
        SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);

        try {
            Log.i(LOGTAG, "E @ " + jsonObj.toString() + "\n");
            JSONArray tire_samples = jsonObj.getJSONArray("tire_samples");
            int num_tire_samples = tire_samples.length();

            /* Find Tablelayout defined in main.xml */
            TableLayout tl = (TableLayout) findViewById(R.id.tirestamps_table);

            for (int i = 0; i < num_tire_samples; i++) {
                TableRow tr = new TableRow(this);
                tr.setLayoutParams(new TableRow.LayoutParams(
                        TableRow.LayoutParams.MATCH_PARENT,
                        TableRow.LayoutParams.MATCH_PARENT)
                );

                // parse response, and add rows to UI panel
                JSONObject tire_sample = tire_samples.getJSONObject(i);
                double psi = tire_sample.getDouble("psi");
                String tm = tire_sample.getString("sample_time");

                TextView tire_press = new TextView(this);
                boolean showPsi = sharedPrefs.getString("units_pressure", getString(R.string.psi)).equals(getString(R.string.psi));
                tire_press.setText("" + (showPsi ? psi + " " + getString(R.string.psi) : (psi / 0.145037738) + " " + getString(R.string.kpa)));
                tr.addView(tire_press);

                TextView tire_sample_time = new TextView(this);
                tire_sample_time.setText(tm);
                tr.addView(tire_sample_time);

                /* Add row to TableLayout. */
                tl.addView(tr, new TableLayout.LayoutParams(
                        TableLayout.LayoutParams.MATCH_PARENT,
                        TableLayout.LayoutParams.MATCH_PARENT)
                );
            }
        } catch (JSONException e) {
            ACRA.getErrorReporter().handleSilentException(null);
        }
    }

}
