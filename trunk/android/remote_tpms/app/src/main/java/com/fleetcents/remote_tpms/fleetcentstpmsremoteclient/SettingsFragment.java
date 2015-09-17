package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.os.Bundle;
import android.preference.PreferenceFragment;


/**
 * Created by bmck on 8/7/15.
 */

public class SettingsFragment extends PreferenceFragment {
        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);

            // Load the preferences from an XML resource
            addPreferencesFromResource(R.xml.pref_basestation);
            addPreferencesFromResource(R.xml.pref_units);
            addPreferencesFromResource(R.xml.pref_crash_report);
            addPreferencesFromResource(R.xml.pref_about);
        }

}
