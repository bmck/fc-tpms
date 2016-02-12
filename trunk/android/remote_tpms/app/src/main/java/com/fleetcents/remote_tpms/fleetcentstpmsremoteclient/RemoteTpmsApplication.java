package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

/**
 * Created by bmck on 9/17/15.
 */

import android.app.AlertDialog;
import android.app.Application;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.Uri;
import android.os.Environment;
import android.util.Log;
import android.widget.Toast;

import org.acra.*;
import org.acra.annotation.*;
import org.acra.sender.HttpSender;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.HashMap;

@ReportsCrashes(
        socketTimeout = 10000,
        httpMethod = HttpSender.Method.PUT,
        reportType = HttpSender.Type.JSON,
        formUri = "http://www.fleetcents.com:5984/acra-remotetpms/_design/acra-storage/_update/report",
        formUriBasicAuthLogin = "remotetpms_reporter",
        formUriBasicAuthPassword = "w4r4w4Rjp"
)

public class RemoteTpmsApplication extends Application {
    private static final String LOGTAG = "RemoteTpmsApplication";

    @Override
    public void onCreate() {
        super.onCreate();
        init();

        HashMap<String, String> ACRAData = new HashMap<String, String>();
        ACRA.getErrorReporter().addReportSender(new ACRAPostSender(ACRAData));
    }

    private void init() {
//        checkAppVersion();

        ACRA.init(this);
        FcQueryService.init(this);
    }


    private void checkAppVersion() {
        String processTarget = getApplicationContext().getString(R.string.processTarget);
        String market = getApplicationContext().getString(R.string.market);
        String appName = "local-" + market + "-" + processTarget + "-debug.apk";

        String fleetUri = "/api/v1/versions.json";
        String fleetParms = "meth=POST";

        String arguments = fleetUri + "?apk=" + appName + "&" + fleetParms;

        Intent mServiceIntent = new Intent(this, FcQueryService.class);
        mServiceIntent.setData(Uri.parse("fc://" + arguments));
        startService(mServiceIntent);
    }

    protected void handleServerResponse(JSONObject jsonObj) {
        class Version implements Comparable<Version> {
            private String version;

            public final String get() {
                return this.version;
            }

            public Version(String version) {
                if (version == null)
                    throw new IllegalArgumentException("Version can not be null");
                if (!version.matches("[0-9]+(\\.[0-9]+)*"))
                    throw new IllegalArgumentException("Invalid version format");
                this.version = version;
            }

            @Override
            public int compareTo(Version that) {
                if (that == null)
                    return 1;
                String[] thisParts = this.get().split("\\.");
                String[] thatParts = that.get().split("\\.");
                int length = Math.max(thisParts.length, thatParts.length);
                for (int i = 0; i < length; i++) {
                    int thisPart = i < thisParts.length ?
                            Integer.parseInt(thisParts[i]) : 0;
                    int thatPart = i < thatParts.length ?
                            Integer.parseInt(thatParts[i]) : 0;
                    if (thisPart < thatPart)
                        return -1;
                    if (thisPart > thatPart)
                        return 1;
                }
                return 0;
            }

            @Override
            public boolean equals(Object that) {
                if (this == that)
                    return true;
                if (that == null)
                    return false;
                if (this.getClass() != that.getClass())
                    return false;
                return this.compareTo((Version) that) == 0;
            }
        }

        String latest_version = "";
        try {
            JSONArray versions = jsonObj.getJSONArray("versions");
            int num_versions = versions.length();
            if (num_versions < 1) {
                return;
            }
            latest_version = versions.getJSONObject(0).getString("v");
            if (num_versions > 1) {
                for (int i = 1; i < num_versions; i++) {
                    String v = versions.getJSONObject(i).getString("v");
                    latest_version = ((new Version(latest_version)).compareTo(new Version(v)) >= 0) ? latest_version : v;
                }
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }

        PackageManager packageManager = getApplicationContext().getPackageManager();
        PackageInfo packageInfo = null;
        try {
            packageInfo = packageManager.getPackageInfo(getApplicationContext().getPackageName(), 0);
        } catch (PackageManager.NameNotFoundException e) {
            e.printStackTrace();
        }
        String this_version = packageInfo.versionName;

        if ((new Version(latest_version)).compareTo(new Version(this_version)) > 0) {
            // get new version of apk
        }

    }
}
