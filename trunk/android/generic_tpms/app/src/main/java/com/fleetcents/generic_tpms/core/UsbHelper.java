/*
 * rtl_tcp_andro is an Android port of the famous rtl_tcp driver for 
 * RTL2832U based USB DVB-T dongles. It does not require root.
 * Copyright (C) 2012 by Martin Marinov <martintzvetomirov@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

package com.fleetcents.generic_tpms.core;

import java.io.File;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.HashMap;
import java.util.HashSet;

import com.fleetcents.generic_tpms.R;
import com.fleetcents.generic_tpms.MainActivity;
import com.fleetcents.generic_tpms.core.RtlSdrStartException.err_info;

import org.xmlpull.v1.XmlPullParser;

import android.annotation.SuppressLint;
import android.content.Context;
import android.content.Intent;
import android.content.res.XmlResourceParser;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.util.AttributeSet;
import android.util.Xml;
import android.util.Log;

public class UsbHelper {

    public static boolean force_root = false;
    private static final String LOGTAG = "UsbHelper";
    private static final String TAG = "FleetCents";

    private final static String DEFAULT_USPFS_PATH = "/dev/bus/usb";

    public enum STATUS {SHOW_DEVICE_DIALOG, REQUESTED_OPEN_DEVICE, CANNOT_FIND, CANNOT_FIND_TRY_ROOT};

    public static HashSet<String> getAllowedDeviceData(final Context ctx) {
        final HashSet<String> ans = new HashSet<String>();
        try {
            final XmlResourceParser xml = ctx.getResources().getXml(R.xml.device_filter);

            xml.next();
            int eventType;
            while ((eventType = xml.getEventType()) != XmlPullParser.END_DOCUMENT) {

                switch (eventType) {
                    case XmlPullParser.START_TAG:
                        if (xml.getName().equals("usb-device")) {
                            final AttributeSet as = Xml.asAttributeSet(xml);
//                            final Integer vendorId = Integer.valueOf(as.getAttributeValue(null, "vendor-id"), 16);
//                            final Integer productId = Integer.valueOf(as.getAttributeValue(null, "product-id"), 16);
//                          Let's assume that these are decimal values until we see otherwise
                            // If they really are hex values, then code in MainActivity @ ~line 216 should be modified
                            final Integer vendorId = Integer.valueOf(as.getAttributeValue(null, "vendor-id"), 10);
                            final Integer productId = Integer.valueOf(as.getAttributeValue(null, "product-id"), 10);
                            String dat = "v" + vendorId + "p" + productId;
//                            Log.i(LOGTAG, "valid dat = " + dat);
                            ans.add(dat);
                        }
                        break;
                }
                xml.next();
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return ans;
    }

    public static UsbDevice findDevice(MainActivity activity) {
        // auto selection by enumeration
        UsbDevice device = null;
        final UsbManager manager = (UsbManager) activity.getSystemService(Context.USB_SERVICE);
        final HashSet<String> allowed = UsbHelper.getAllowedDeviceData(activity);
        final HashMap<String, UsbDevice> deviceList = manager.getDeviceList();

        if (deviceList.isEmpty()) {
            throw new RuntimeException(String.valueOf(R.string.exception_NO_DEVICES));
//            return STATUS.CANNOT_FIND;
        }

        for (final String desc : deviceList.keySet()) {
            final UsbDevice candidate = deviceList.get(desc);
            final String candstr = "v" + candidate.getVendorId() + "p" + candidate.getProductId();
//            Log.i(LOGTAG, "candidate str = " + candstr);
            if (allowed.contains(candstr)) {
                device = candidate;
            }
        }

        if (device == null) {
            throw new RuntimeException(String.valueOf(R.string.exception_NO_DEVICES));
        }

        return device;
    }

    public static UsbDeviceConnection openDevice(MainActivity activity, final UsbDevice device) {
        final UsbManager manager = (UsbManager) activity.getSystemService(Context.USB_SERVICE);

        if (device != null && !manager.hasPermission(device)) {
            boolean mPermissionRequestPending = false;
            android.util.Log.i(LOGTAG, "No permissions for device, requesting!");
            while (!manager.hasPermission(device)) {
                synchronized (manager) {
                    if (!mPermissionRequestPending) {
                        manager.requestPermission(device, activity.permissionIntent);
                        mPermissionRequestPending = true;
                    }
                }
            }
        }
//            android.util.Log.i(LOGTAG, "manager = " + manager);
//            android.util.Log.i(LOGTAG, "device = " + device);
        if ((device == null) || (!manager.hasPermission(device))) {
            activity.finishWithError(err_info.permission_denied);
//                android.util.Log.i(LOGTAG, "here");
        }
//                android.util.Log.i(LOGTAG, "manager = " + manager);
//                android.util.Log.i(LOGTAG, "device = " + device);
        final UsbDeviceConnection connection = manager.openDevice(device);

        if (connection == null) {
            activity.finishWithError(err_info.unknown_error);
        } else {
            return connection;
        }

        return null;
    }

    public final static String properDeviceName(String deviceName) {
        if (deviceName == null) return DEFAULT_USPFS_PATH;
        deviceName = deviceName.trim();
        if (deviceName.isEmpty()) return DEFAULT_USPFS_PATH;

        final String[] paths = deviceName.split("/");
        final StringBuilder sb = new StringBuilder();
        for (int i = 0; i < paths.length - 2; i++)
            if (i == 0)
                sb.append(paths[i]);
            else
                sb.append("/" + paths[i]);
        final String stripped_name = sb.toString().trim();
        if (stripped_name.isEmpty())
            return DEFAULT_USPFS_PATH;
        else
            return stripped_name;
    }

    private static void chmodRecursive(final String dir, final OutputStreamWriter osw) throws IOException {
        osw.write("chmod 777 " + dir + "\n");
        osw.flush();

        final String[] files = new File(dir).list();
        if (files == null) return;
        for (final String s : files) {
            final String fname = dir + s;
            final File f = new File(fname);

            if (f.isDirectory())
                chmodRecursive(fname + "/", osw);
            else {
                osw.write("chmod 777 " + fname + "\n");
                osw.flush();
            }
        }
    }

    public static void fixRootPermissions() throws RtlSdrStartException {
        Runtime runtime = Runtime.getRuntime();
        OutputStreamWriter osw = null;
        Process proc = null;
        try { // Run Script

            proc = runtime.exec("su");

            osw = new OutputStreamWriter(proc.getOutputStream());

            chmodRecursive("/dev/bus/usb/", osw);

            osw.close();

        } catch (IOException ex) {
            throw new RtlSdrStartException(err_info.root_required);
        } finally {
            if (osw != null) {
                try {
                    osw.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        try {
            if (proc != null)
                proc.waitFor();
        } catch (InterruptedException e) {
        }

        if (proc.exitValue() != 0) {
            Log.i(LOGTAG, "Root refused to give permissions.");
            throw new RtlSdrStartException(err_info.permission_denied);
        }
    }
}
