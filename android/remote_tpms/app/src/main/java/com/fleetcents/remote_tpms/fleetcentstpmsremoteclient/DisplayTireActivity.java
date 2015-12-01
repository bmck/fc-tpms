package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.widget.RelativeLayout;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;

import org.acra.ACRA;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class DisplayTireActivity extends AbstractClientActivity {

    protected String tireSampleUri = "api/v1/tire_samples.json";
    protected String tireSampleParms = "tire_id=";

    private int tireId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_tire);
    }

    protected void onResume() {
        super.onResume();

        RelativeLayout layout = (RelativeLayout) findViewById(R.id.display_fleet_layout);

        TextView fleet_instructs = (TextView) findViewById(R.id.tirestamp_dirs);
        fleet_instructs.setInputType(0x00020001);

        if ((getString(R.string.userid).length() > 0) && (getString(R.string.password).length() > 0)) {
            fleet_instructs.setText(getString(R.string.tire_samples_instructs));

            String prms = "";

            Bundle extras = getIntent().getExtras();
            if (extras != null) {
                tireId = extras.getInt("EXTRA_TIRE_ID");
                prms = tireSampleParms + tireId;
            }

            callToServer(tireSampleUri, prms, "GET");
        }
        else {
            fleet_instructs.setText(getString(R.string.update_creds));
        }
    }

    protected void handleServerResponse(JSONObject jsonObj) {
        // parse response, and add buttons to UI panel
        SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);

        try {
        JSONArray tire_samples = jsonObj.getJSONArray("tire_samples");
        int num_tire_samples = tire_samples.length();

            /* Find Tablelayout defined in main.xml */
            TableLayout tl = (TableLayout) findViewById(R.id.vehicle_tires_table);

            for (int i = 0; i < num_tire_samples; i++) {
                TableRow tr = new TableRow(this);
                tr.setLayoutParams(new TableRow.LayoutParams(TableRow.LayoutParams.MATCH_PARENT, TableRow.LayoutParams.MATCH_PARENT));

                // parse response, and add rows to UI panel
                JSONObject tire_sample = tire_samples.getJSONObject(i);
                double psi = tire_sample.getDouble("psi");
                String tm = tire_sample.getString("sample_time");

                TextView tire_press = new TextView(this);
                boolean showPsi = sharedPrefs.getString("units_pressure", getString(R.string.psi)).equals(getString(R.string.psi));
                tire_press.setText("" + (showPsi ? psi + " " + getString(R.string.psi) : (psi/0.145037738) + " " + getString(R.string.kpa)));
                tr.addView(tire_press);

                TextView tire_sample_time = new TextView(this);
                tire_sample_time.setText(tm);
                tr.addView(tire_sample_time);

                /* Add row to TableLayout. */
                tl.addView(tr, new TableLayout.LayoutParams(TableLayout.LayoutParams.MATCH_PARENT, TableLayout.LayoutParams.MATCH_PARENT));
            }
        }
        catch (JSONException e) {
            ACRA.getErrorReporter().handleSilentException(null);
        }
    }

}
