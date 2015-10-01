package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

/**
 * Created by bmck on 9/17/15.
 */
import android.app.Application;
import android.content.Context;
import android.util.Log;

import org.acra.*;
import org.acra.annotation.*;
import org.acra.sender.HttpSender;

import java.util.HashMap;

@ReportsCrashes(
        socketTimeout = 10000,
        httpMethod = HttpSender.Method.PUT,
        reportType = HttpSender.Type.JSON,
        formUri = "http://server11288.baremetalcloud.com:5984/acra-remotetpms/_design/acra-storage/_update/report",
        formUriBasicAuthLogin = "remotetpms_reporter",
        formUriBasicAuthPassword = "w4r4w4Rjp"
)
public class RemoteTpmsApplication extends Application {
    private static final String LOGTAG = "RemoteTpmsApplication";

    @Override
    public void onCreate() {
        super.onCreate();
        ACRA.init(this);

        HashMap<String,String> ACRAData = new HashMap<String,String>();
//        ACRAData.put("my_app_info", "custom data");
        ACRA.getErrorReporter().addReportSender(new ACRAPostSender(ACRAData));
    }
}
