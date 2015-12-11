package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.graphics.Color;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v4.content.LocalBroadcastManager;
import android.text.format.Time;
import android.util.Log;
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

public class DisplayVehicleActivity extends AbstractBaseActivity {

    private static final String LOGTAG = "DisplayVehicleActivity";
    protected String vehicleUri = "/api/v1/tires.json";
    protected String vehicleParms = "truck_id=";

    private int truckId;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_vehicle);
    }

    private BroadcastReceiver bReceiver = new BroadcastReceiver() {
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
            truckId = extras.getInt("EXTRA_TRUCK_ID");
            prms = prms + "&" + vehicleParms + truckId;
        }

        String arguments = vehicleUri + "?" + prms;
        LocalBroadcastManager.getInstance(this).registerReceiver(bReceiver, new IntentFilter(arguments));

        RelativeLayout layout = (RelativeLayout) findViewById(R.id.display_fleet_layout);

        TextView vehicle_instructs = (TextView) findViewById(R.id.vehicle_dirs);
        vehicle_instructs.setInputType(0x00020001);

        if (credsProvided) {
            if (!serviceCallHandled) {
                serviceCallHandled = true;

                vehicle_instructs.setTextSize(16);
                vehicle_instructs.setText(getString(R.string.vehicle_instructs));

//            callToServer(vehicleUri, prms, "GET");
                Intent mServiceIntent = new Intent(this, FcQueryService.class);
                mServiceIntent.setData(Uri.parse("fc://" + arguments));
                startService(mServiceIntent);
            }
        } else {
            vehicle_instructs.setTextSize(24);
            vehicle_instructs.setText(getString(R.string.update_creds));
        }
    }

    protected void handleServerResponse(JSONObject jsonObj) {
        Log.i(LOGTAG, "Calling " + LOGTAG + "handleServerResponse\n");

        SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(this);
        RelativeLayout layout = (RelativeLayout) findViewById(R.id.display_vehicle_layout);
        int grn = (R.color.fc_green);
        int ylo = (R.color.fc_yellow);
        int red = (R.color.fc_red);

        try {
            Log.i(LOGTAG, "D @ " + jsonObj.toString() + "\n");
            // parse response, and add buttons to UI panel
            JSONArray tires = jsonObj.getJSONArray("tires");
            int num_tires = (tires.length() > 4) ? 4 : tires.length();

            /* Find Tablelayout defined in main.xml */
            TableLayout tl = (TableLayout) findViewById(R.id.vehicle_tires_table);
            TableRow hdr_tr = new TableRow(this);

            TextView tire_color_hdr = new TextView(this);
            hdr_tr.addView(tire_color_hdr);

            TextView tire_hdr = new TextView(this);
            tire_hdr.setText("Tires");
            tire_hdr.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
            hdr_tr.addView(tire_hdr);

            TextView press_hdr = new TextView(this);
            press_hdr.setText("Sample\nPressure");
            press_hdr.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
            hdr_tr.addView(press_hdr);

            TextView time_hdr = new TextView(this);
            time_hdr.setText("Sample Times");
            time_hdr.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
            hdr_tr.addView(time_hdr);

            TextView btn_hdr = new TextView(this);
            hdr_tr.addView(btn_hdr);

            tl.addView(hdr_tr, new TableLayout.LayoutParams(
                    TableLayout.LayoutParams.MATCH_PARENT,
                    TableLayout.LayoutParams.MATCH_PARENT
            ));


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
                tr.setLayoutParams(new TableRow.LayoutParams(
                        TableRow.LayoutParams.MATCH_PARENT,
                        TableRow.LayoutParams.MATCH_PARENT
                ));

                TextView tire_color = new TextView(this);
                int col = TireColorizer.getInstance().getColor(psi, hi_psi, lo_psi, tm, tm_thresh);

                tire_color.setBackgroundResource(col);
                tr.addView(tire_color);

                TextView tire_desc = new TextView(this);
                tire_desc.setText(tire.getString("tire_type") + " with Sensor #" + tire.getString("sensor_id"));
                tr.addView(tire_desc);

                TextView tire_press = new TextView(this);
                boolean showPsi = sharedPrefs.getString("units_pressure", getString(R.string.psi)).equals(getString(R.string.psi));
                tire_press.setText("" + (showPsi ? psi + " " + getString(R.string.psi) : (psi / 0.145037738) + " " + getString(R.string.kpa)));
                tr.addView(tire_press);

                TextView tire_sample_time = new TextView(this);
                Date date = new Date(tm * 1000L); // *1000 is to convert seconds to milliseconds
                SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss z"); // the format of your date
                tire_sample_time.setText(sdf.format(date));
                tr.addView(tire_sample_time);

                Button b = new Button(this);
                b.setText("Timestamps");
                b.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
                b.setId(tire_id);

                b.setOnClickListener(new View.OnClickListener() {
                    public void onClick(View v) {
                        Intent i = new Intent(getApplicationContext(), DisplayTireActivity.class);
                        i.putExtra("EXTRA_TIRE_ID", v.getId());
                        startActivity(i);
                    }
                });

                tr.addView(b);

                /* Add row to TableLayout. */
                tl.addView(tr, new TableLayout.LayoutParams(TableLayout.LayoutParams.MATCH_PARENT, TableLayout.LayoutParams.MATCH_PARENT));
            }
        } catch (JSONException e) {
            ACRA.getErrorReporter().handleSilentException(null);
        }
    }
}
