package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.text.Html;
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
            // TODO Auto-generated catch block
            e.printStackTrace();
        }

        TextView aboutTextView = (TextView) findViewById(R.id.about_text_view);

        String gpls = "libusb-andro\n\n" +
                getResources().openRawResource(R.raw.libusbandro) +
                "\n\n" +
                "rtl-sdr\n\n" +
                getResources().openRawResource(R.raw.rtlsdr);
//        BufferedReader r = new BufferedReader(new InputStreamReader(is));
//        StringBuilder total = new StringBuilder();
//        String line;
//        try {
//            while ((line = r.readLine()) != null) {
//                total.append(line);
//            }
//        } catch (IOException e) {
//            e.printStackTrace();
//        }

        String aboutText = "Your App Name, Version " + versionName + "\n\n" +
                "Included Packages and Relevant Licenses\n\n"
                + gpls;
        aboutTextView.setText(aboutText);
    }

}
