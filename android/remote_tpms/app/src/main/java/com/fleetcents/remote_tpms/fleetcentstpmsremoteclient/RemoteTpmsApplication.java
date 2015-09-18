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

//        Log.i(LOGTAG, "\n\n\ntesting acra");
//        ACRA.getErrorReporter().handleSilentException(null);
//        Log.i(LOGTAG, "end test\n\n\n");
    }
}
