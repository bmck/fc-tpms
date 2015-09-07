package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.os.Bundle;
import android.preference.PreferenceFragment;

/**
 * Created by bmck on 9/1/15.
 */
public class UnitSettingsFragment extends PreferenceFragment {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Load the preferences from an XML resource
        addPreferencesFromResource(R.xml.pref_about);
    }

}
