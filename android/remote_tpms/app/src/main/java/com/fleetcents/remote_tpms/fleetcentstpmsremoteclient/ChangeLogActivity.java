package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.widget.TextView;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * Created by bmck on 8/23/15.
 */
public class ChangeLogActivity extends Activity {
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.about_layout);

        String versionName = getString(R.string.version);
        try {
            PackageInfo pinfo = getPackageManager().getPackageInfo(getPackageName(), 0);
            versionName = pinfo.versionName;
        } catch (PackageManager.NameNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        TextView aboutTextView = (TextView) findViewById(R.id.about_text_view);

        InputStream is = getResources().openRawResource(R.raw.changelog);
        BufferedReader r = new BufferedReader(new InputStreamReader(is));
        StringBuilder total = new StringBuilder();
        String line;
        try {
            while ((line = r.readLine()) != null) {
                total.append(line + "\n");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        String changelog = "\n\n" + total.toString() + "\n";

        String aboutText = getString(R.string.app_name) + "\n" +
                "Version " + getString(R.string.version) + "\n\n" +
                "ChangeLog\n" + changelog;
        aboutTextView.setText(aboutText);
    }
}
