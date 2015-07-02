package com.fleetcents.generic_tpms.core;

import android.widget.TextView;

import com.fleetcents.generic_tpms.MainActivity;
import com.fleetcents.generic_tpms.R;

/**
 * Created by bmck on 7/1/15.
 */
public class FcTextView extends TextView {

    public FcTextView(MainActivity act) {
        super(act);
        append(act.getString(R.string.hello_world) + "\n");
    }
}
