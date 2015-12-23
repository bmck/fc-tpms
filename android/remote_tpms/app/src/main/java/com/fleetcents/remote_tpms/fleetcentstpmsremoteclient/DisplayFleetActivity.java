package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.android.volley.toolbox.Volley;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.text.SimpleDateFormat;
import java.util.Date;


public class DisplayFleetActivity extends AbstractBaseActivity {

    private static final String LOGTAG = "DisplayFleetActivity";
    protected String fleetUri = "/api/v1/trucks.json";
    protected String fleetParms = "meth=GET";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_fleet);
    }

    private BroadcastReceiver bReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            String response = intent.getStringExtra("response");
            Log.i(LOGTAG, "response = " + response);
            try {
                JSONObject jsonObj = new JSONObject(response);
                handleServerResponse(jsonObj);
            } catch (JSONException e) {
                e.printStackTrace();
            }
        }
    };

    @Override
    protected void onResume() {
        super.onResume();
        String arguments = fleetUri + "?" + fleetParms;
        LocalBroadcastManager.getInstance(this).registerReceiver(bReceiver, new IntentFilter(arguments));

        RelativeLayout layout = (RelativeLayout) findViewById(R.id.display_fleet_layout);

        TextView fleet_instructs = (TextView) findViewById(R.id.fleet_dirs);
        fleet_instructs.setInputType(0x00020001);

        if (credsProvided) {
            if (!serviceCallHandled) {
                serviceCallHandled = true;

                fleet_instructs.setTextSize(16);
                fleet_instructs.setText(getString(R.string.fleet_instructs));

                Intent mServiceIntent = new Intent(this, FcQueryService.class);
                mServiceIntent.setData(Uri.parse("fc://" + arguments));
                startService(mServiceIntent);
            }
        } else {
            fleet_instructs.setTextSize(24);
            fleet_instructs.setText(getString(R.string.update_creds));
        }
    }

    protected void onPause() {
        super.onPause();
        LocalBroadcastManager.getInstance(this).unregisterReceiver(bReceiver);
    }

    protected void handleServerResponse(JSONObject jsonObj) {
        LinearLayout layout = (LinearLayout) findViewById(R.id.fleet_buttons);

        try {
            // parse response, and add buttons to UI panel
            JSONArray trucks = jsonObj.getJSONArray("trucks");
            int num_trucks = trucks.length();

            for (int i = 0; i < num_trucks; i++) {
                JSONObject truck = trucks.getJSONObject(i);
                String truck_name = truck.getString("name");
                int truck_id = truck.getInt("id");

                //set the properties for button
                Button b = new Button(this);
                b.setLayoutParams(
                        new ViewGroup.LayoutParams(
                                ViewGroup.LayoutParams.MATCH_PARENT,
                                ViewGroup.LayoutParams.WRAP_CONTENT
                        )
                );
                b.setText(truck_name);
                b.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
                b.setTextSize(16);
                b.setTypeface(b.getTypeface(), Typeface.BOLD);
                b.setId(truck_id);
                b.setOnClickListener(new View.OnClickListener() {
                    public void onClick(View v) {
                        Intent i = new Intent(getApplicationContext(), DisplayVehicleActivity.class);
                        i.putExtra("EXTRA_TRUCK_ID", v.getId());
                        startActivity(i);
                    }
                });

                //add button to the layout
                layout.addView(b);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }
}