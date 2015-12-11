package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends Activity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void set_user_creds(View v) {
        Intent i = new Intent(getApplicationContext(), LoginActivity.class);
        startActivity(i);
    }

    public void display_fleet_activity(View v) {
        Intent i = new Intent(getApplicationContext(), DisplayFleetActivity.class);
        startActivity(i);
    }

    public void collect_tire_sample(View v) {
        Intent i = new Intent(getApplicationContext(), LogActivity.class);
        startActivity(i);
    }
}
