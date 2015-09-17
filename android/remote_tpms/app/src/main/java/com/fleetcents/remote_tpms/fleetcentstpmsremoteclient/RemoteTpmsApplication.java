package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

/**
 * Created by bmck on 9/17/15.
 */
import org.acra.*;
import org.acra.annotation.*;

@ReportsCrashes(
//        formKey = "", // This is required for backward compatibility but not used
        formUri = "https://collector.tracepot.com/5fe4fecb",
        socketTimeout = 10000,
        mode = ReportingInteractionMode.SILENT
)
public class RemoteTpmsApplication extends android.app.Application {
    @Override
    public void onCreate() {
        super.onCreate();
        ACRA.init(this);
    }
}
