package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

/**
 * Created by bmck on 9/7/15.
 */
public class RtlSdrFeedbackTypes {
    // Defines a custom Intent action
    public static final String BROADCAST_ACTION =
            "com.fleetcents.remote_tpms.fleetcentstpmsremoteclient.BROADCAST";
    // Defines the key for the status "extra" in an Intent
    public static final String EXTENDED_DATA_STATUS =
            "com.fleetcents.remote_tpms.fleetcentstpmsremoteclient.STATUS";

    public static final String EXTENDED_DATA_FAILURE =
            "com.fleetcents.remote_tpms.fleetcentstpmsremoteclient.FAILURE";

    public static final String EXTENDED_DATA_COMPLETEOK =
            "com.fleetcents.remote_tpms.fleetcentstpmsremoteclient.COMPLETEOK";

}
