package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;


public class DisplayFleetActivity extends AbstractClientActivity implements View.OnClickListener {

    protected String fleetUri = "api/v1/trucks.json";
    protected String fleetParms = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_display_fleet);
    }

    @Override
    protected void onResume() {
        super.onResume();

        RelativeLayout layout = (RelativeLayout) findViewById(R.id.display_fleet_layout);

        TextView fleet_instructs = (TextView) findViewById(R.id.fleet_dirs);
        fleet_instructs.setInputType(0x00020001);

        if ((getString(R.string.userid).length() > 0) && (getString(R.string.password).length() > 0)) {
            fleet_instructs.setText(getString(R.string.fleet_instructs));

            callToServer(fleetUri, fleetParms, "GET");
        }
        else {
            fleet_instructs.setText(getString(R.string.update_creds));
        }
    }

    protected void handleServerResponse(JSONObject jsonObj) {
        //the layout on which you are working
        RelativeLayout layout = (RelativeLayout) findViewById(R.id.display_fleet_layout);

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
                                ViewGroup.LayoutParams.WRAP_CONTENT,
                                ViewGroup.LayoutParams.WRAP_CONTENT
                        )
                );
                b.setText(truck_name);
                b.setId(truck_id);

                //add button to the layout
                layout.addView(b);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    public void onClick(View v) {
        Intent i = new Intent(getApplicationContext(), DisplayVehicleActivity.class);
        i.putExtra("EXTRA_TRUCK_ID", v.getId());
        startActivity(i);
    }
}