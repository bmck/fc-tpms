package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.os.Bundle;
import android.preference.PreferenceFragment;


/**
 * Created by bmck on 8/7/15.
 */

// TODO: Validate the secssent setting being 0.1<=x<=5.0
// TODO: Add settings to allow user to choose how to display tire data
// TODO: Add app version in settings panel
public class BaseStationSettingsFragment extends PreferenceFragment {
        @Override
        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);

            // Load the preferences from an XML resource
            addPreferencesFromResource(R.xml.pref_basestation);
        }

}
