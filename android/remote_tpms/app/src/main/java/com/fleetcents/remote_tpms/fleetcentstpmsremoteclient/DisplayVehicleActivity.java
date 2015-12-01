package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.content.Intent;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.text.format.Time;
import android.view.View;
import android.widget.Button;
import android.widget.RelativeLayout;
import android.widget.TableLayout;
import android.widget.TableRow;
import android.widget.TextView;

import org.acra.ACRA;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class DisplayVehicleActivity extends AbstractClientActivity {

    protected String vehicleUri = "api/v1/tires.json";
    protected String vehicleParms = "tire_id=";

    private int truckId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_vehicle);
    }

    protected void onResume() {
        super.onResume();

        RelativeLayout layout = (RelativeLayout) findViewById(R.id.display_fleet_layout);

        TextView fleet_instructs = (TextView) findViewById(R.id.vehicle_dirs);
        fleet_instructs.setInputType(0x00020001);

        if ((getString(R.string.userid).length() > 0) && (getString(R.string.password).length() > 0)) {
            fleet_instructs.setText(getString(R.string.vehicle_instructs));

            String prms = "";

            Bundle extras = getIntent().getExtras();
            if (extras != null) {
                truckId = extras.getInt("EXTRA_TRUCK_ID");
                prms = vehicleParms + truckId;
            }

            callToServer(vehicleUri, prms, "GET");
        }
        else {
            fleet_instructs.setText(getString(R.string.update_creds));
        }
    }

    protected void handleServerResponse(JSONObject jsonObj) {
        SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);
        RelativeLayout layout = (RelativeLayout) findViewById(R.id.display_vehicle_layout);

        try {
            // parse response, and add buttons to UI panel
            JSONArray tires = jsonObj.getJSONArray("tires");
            int num_tires = (tires.length() > 4) ? 4 : tires.length();

            /* Find Tablelayout defined in main.xml */
            TableLayout tl = (TableLayout) findViewById(R.id.vehicle_tires_table);

            for (int i = 0; i < num_tires; i++) {
                JSONObject tire = tires.getJSONObject(i);
                int tire_id = tire.getInt("id");
                double psi = tire.getDouble("last_psi");
                long tm = tire.getLong("last_sample_time");
                long tm_thresh = tire.getLong("time_threshold");
                double hi_psi = tire.getDouble("hi_psi_threshold");
                double lo_psi = tire.getDouble("lo_psi_threshold");

                /* Create a new row to be added. */
                TableRow tr = new TableRow(this);
                tr.setLayoutParams(new TableRow.LayoutParams(TableRow.LayoutParams.MATCH_PARENT, TableRow.LayoutParams.MATCH_PARENT));

                TextView tire_color = new TextView(this);
                int grn = Color.GREEN;
                int ylo = Color.YELLOW;
                int red = Color.RED;
                int col = grn;

                Calendar c = Calendar.getInstance();
                int seconds = c.get(Calendar.SECOND);

                if (tm < seconds - tm_thresh) {
                    col = ylo;
                }
                if (hi_psi > 0.0) {
                    if (psi > hi_psi) {
                        col = red;
                    }
                }
                if (lo_psi > 0.0) {
                    if (psi < lo_psi) {
                        col = red;
                    }
                }

                tire_color.setBackgroundResource(col);
                tr.addView(tire_color);

                TextView tire_desc = new TextView(this);
                tire_desc.setText(tire.getString("tire_type") + " with Sensor #" + tire.getString("sensor_id"));
                tr.addView(tire_desc);

                TextView tire_press = new TextView(this);
                boolean showPsi = sharedPrefs.getString("units_pressure", getString(R.string.psi)).equals(getString(R.string.psi));
                tire_press.setText("" + (showPsi ? psi + " " + getString(R.string.psi) : (psi/0.145037738) + " " + getString(R.string.kpa)));
                tr.addView(tire_press);

                TextView tire_sample_time = new TextView(this);
                Date date = new Date(tm * 1000L); // *1000 is to convert seconds to milliseconds
                SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss z"); // the format of your date
                tire_sample_time.setText(sdf.format(date));
                tr.addView(tire_sample_time);

                Button b = new Button(this);
                b.setText("Timestamps");
                b.setId(tire_id);
                tr.addView(b);

                /* Add row to TableLayout. */
                tl.addView(tr, new TableLayout.LayoutParams(TableLayout.LayoutParams.MATCH_PARENT, TableLayout.LayoutParams.MATCH_PARENT));
            }
        } catch (JSONException e) {
            ACRA.getErrorReporter().handleSilentException(null);
        }
    }
    public void onClick(View v) {
        Intent i = new Intent(getApplicationContext(), DisplayTireActivity.class);
        i.putExtra("EXTRA_TIRE_ID", v.getId());
        startActivity(i);
    }
}
