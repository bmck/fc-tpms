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

public class LegalActivity extends Activity {
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        setContentView(R.layout.about_layout);

        String versionName = getString(R.string.version);
        try {
            PackageInfo pinfo = getPackageManager().getPackageInfo(getPackageName(), 0);
            versionName = pinfo.versionName;
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }

        TextView aboutTextView = (TextView) findViewById(R.id.about_text_view);

        InputStream is = getResources().openRawResource(R.raw.libusbandro_lgpl_raw);
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

        String gpls = "\nlibusb-andro\n\n" + total.toString() + "\n\n";

        is = getResources().openRawResource(R.raw.rtlsdr_gpl_raw);
        r = new BufferedReader(new InputStreamReader(is));
        total = new StringBuilder();
        try {
            while ((line = r.readLine()) != null) {
                total.append(line + "\n");
            }
        } catch (IOException e) {
            e.printStackTrace();
        }

        gpls += "\n" +
                "\n" +
                "\n" +
                "\n" +
                "------------------------\n" +
                "\n" +
                "\n" +
                "\nrtl-sdr\n\n" + total.toString() + "\n\n";

        String aboutText = getString(R.string.app_name) + "\n" +
                "Version " + getString(R.string.version) + "\n\n" +
                "Included Packages and Relevant Licenses\n\n" + gpls;
        aboutTextView.setText(aboutText);
    }

}
