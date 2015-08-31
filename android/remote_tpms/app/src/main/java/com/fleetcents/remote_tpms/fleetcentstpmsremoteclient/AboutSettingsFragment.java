package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.os.Bundle;
import android.preference.PreferenceFragment;

/**
 * Created by bmck on 8/23/15.
 */
public class AboutSettingsFragment extends PreferenceFragment {
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        // Load the preferences from an XML resource
        addPreferencesFromResource(R.xml.pref_about);
    }
}
