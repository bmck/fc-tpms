package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

/**
 * Created by bmck on 12/11/15.
 */
public class TireColorizer {
    private static TireColorizer ourInstance = new TireColorizer();

    public static TireColorizer getInstance() {
        return ourInstance;
    }

    private TireColorizer() {
    }

    public int getColor(double pressure, double hi_press_thresh, double lo_press_thresh,
                        long press_time, long time_thresh) {
        return getColor(pressure, hi_press_thresh, lo_press_thresh,
                press_time, time_thresh, Math.round(System.currentTimeMillis()/1000.0));
    }

    public int getColor(double pressure, double hi_press_thresh, double lo_press_thresh,
                        long press_time, long time_thresh, long eval_time) {
        int grn = (R.color.fc_green);
        int ylo = (R.color.fc_yellow);
        int red = (R.color.fc_red);
        int col = grn;

        if (press_time < eval_time - time_thresh) {
            col = ylo;
        }
        if (hi_press_thresh > 0.0) {
            if (pressure > hi_press_thresh) {
                col = red;
            }
        }
        if (lo_press_thresh > 0.0) {
            if (pressure < lo_press_thresh) {
                col = red;
            }
        }
    return col;
    }
}
