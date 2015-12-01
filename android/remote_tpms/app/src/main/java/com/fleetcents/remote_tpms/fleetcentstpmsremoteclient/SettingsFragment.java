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
            if (getString(R.string.processTarget).equalsIgnoreCase (getString(R.string.local))) {
                addPreferencesFromResource(R.xml.pref_basestation_local);
            }
            else if (getString(R.string.processTarget).equalsIgnoreCase(getString(R.string.remote))) {
                addPreferencesFromResource(R.xml.pref_basestation_remote);
            }
            else {
                addPreferencesFromResource(R.xml.pref_basestation_localremote);
            }
            addPreferencesFromResource(R.xml.pref_units);
            addPreferencesFromResource(R.xml.pref_gain);
            addPreferencesFromResource(R.xml.pref_crash_report);
            addPreferencesFromResource(R.xml.pref_about);
        }

}
