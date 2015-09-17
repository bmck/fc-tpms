package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.IntentService;
import android.content.Intent;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.net.Uri;
import android.support.v4.content.LocalBroadcastManager;
import android.text.TextUtils;
import android.util.Log;

import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import org.apache.commons.net.ftp.FTP;
import org.apache.commons.net.ftp.FTPClient;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.lang.reflect.Array;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;
import java.util.Arrays;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;

/**
 * Created by bmck on 9/7/15.
 */
public class SdrFtpService extends IntentService {

    private static final String LOGTAG = "SdrFtpService";
    private static final String TAG = "FleetCents";
    public String resp;
    MainActivity activity = null;

    public SdrFtpService() {
        super("SdrFtpService");
    }

    @Override
    protected void onHandleIntent(Intent workIntent) {
        // Gets data from the incoming Intent
        String str = workIntent.getDataString();
        Uri u = Uri.parse("rtlsdr://" + str);
        boolean testing = u.getBooleanQueryParameter("t", true);
        boolean cDisplay = u.getBooleanQueryParameter("c", true);
        boolean psiDisplay = u.getBooleanQueryParameter("p", true);
        String fn = u.getQueryParameter("fn");
        int sample_rate = Integer.parseInt(u.getQueryParameter("s"));
        int num_samples = Integer.parseInt(u.getQueryParameter("n"));
        int freq = Integer.parseInt(u.getQueryParameter("f"));
        String base = u.getQueryParameter("base");

        activity = MainActivity.activity;

        String arguments = "-f " + freq + " -s " + sample_rate + " -n " + num_samples +
                " -t " + (testing ? "1" : "0") + " " + fn + "";
        activity.log_it("i", LOGTAG, "SDR data collection arguments: " + arguments);

        synchronized (this) {
            Log.i(LOGTAG, "A @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()) + "\n");
            displayMessage(getString(R.string.msg_send_activation) + " at " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));

            try {
                if (!testing) {
                    UsbDevice device = null;
                    UsbDeviceConnection connection = null;
                    try {
                        device = UsbHelper.findDevice(activity);
                        connection = UsbHelper.openDevice(activity, device);
                    } catch (RuntimeException e) {
                        // TODO: Resolve why the error msgs in UsbHelper do not correctly render?
                        errorModalBox(getString(Integer.parseInt(e.getMessage())));
                        return;
                    }
                    if (connection == null) {
                        errorModalBox(getString(R.string.exception_FAILED_TO_OPEN_DEVICE));
                        return;
                    }

                    displayMessage(getString(R.string.msg_wait_for_sensor_data));

                    activity.log_it("d", LOGTAG, "Before beginning native code at " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                    RtlSdr.open(arguments, connection.getFileDescriptor(), UsbHelper.properDeviceName(device.getDeviceName()));
                    activity.log_it("d", LOGTAG, "Completed native code " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                } else {
                    displayMessage(getString(R.string.msg_wait_for_sensor_data));

                    activity.log_it("d", LOGTAG, "Before beginning native code at " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                    RtlSdr.open(arguments, -1, null);
                    activity.log_it("d", LOGTAG, "Completed native code " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));

                }
                displayMessage(getString(R.string.msg_rcving_sensor_data) + " at " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                Log.i(LOGTAG, "onActivityResult: RTL-SDR Data Capture Completed.");
            } catch (Exception e) {
                activity.finishWithError(e);
                return;
            }

            FTPClient ftpClient = new FTPClient();
            resp = " ,,,";
            int found = 0;
            try {
                String addr = InetAddress.getByName(base).getHostAddress();
                activity.log_it("i", LOGTAG, "connecting to Fleet Cents server at " + addr);
                ftpClient.connect(addr);
                displayMessage(getString(R.string.msg_uploading_sensor_data) + " at " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                // TODO: use password field of login to identify user or hardware being used?
                ftpClient.enterLocalPassiveMode();
                ftpClient.login("anonymous", "guest");
                activity.log_it("i", LOGTAG, "Logged into Fleet Cents server");
                activity.log_it("i", LOGTAG, ftpClient.getReplyString());

                ftpClient.changeWorkingDirectory("fleet_server");
                ftpClient.setFileType(FTP.BINARY_FILE_TYPE);
                BufferedInputStream buffIn = null;
                buffIn = new BufferedInputStream(new FileInputStream(fn));
//                ftpClient.enterLocalPassiveMode();
                activity.log_it("i", LOGTAG, "Before sending file to Fleet Cents server");
                ftpClient.storeUniqueFile(buffIn);
                activity.log_it("i", LOGTAG, "Completed sending file to Fleet Cents server");
                buffIn.close();
                resp = ftpClient.getReplyString();
                activity.log_it("i", LOGTAG, "Complete response to upload: " + resp);
                resp = TextUtils.join(",", Arrays.copyOfRange((resp.split(" ")[1]).split(","), 0, 3));
                activity.log_it("i", LOGTAG, "Relevant response to upload: " + resp);
//                Log.i(LOGTAG, "resp = " + resp);
//                Log.i(LOGTAG, "resp[0] = >" + resp.split(",")[0] + "<");
//                Log.i(LOGTAG, "resp[1] = >" + resp.split(",")[1] + "<");
//                Log.i(LOGTAG, "resp[2] = >" + resp.split(",")[2] + "<");

                try {
                    displayMessage("\n" +
                            new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()) + "\n" +
                            getString(R.string.sensor_id) + " " + getHexAddr() + "\n" +
                            getString(R.string.temp) + " " +
                            (cDisplay ? (getTempC() + " " + getString(R.string.celsius)) : (getTempF() + " " + getString(R.string.fahrenheit))) +
                            "\n" +
                            getString(R.string.pressure) + " " +
                            (psiDisplay ? (getPsi() + " " + getString(R.string.psi)) : (getKpa() + " " + getString(R.string.kpa))) +
                            "\n");
                    found = 1;
                } catch (NumberFormatException e) {
                    found = 0;
                }

                // resp code is standard FTP (unchanged) indicating success/failure
                // resp format is CSV: hex address, tempC, pressure_kpa
                ftpClient.logout();
                ftpClient.disconnect();
            } catch (UnknownHostException e) {
                activity.log_it("e", LOGTAG, "Unknown Host Exception: " + e.toString());
                errorModalBox(getString(R.string.exception_NO_FTP_SERVER));
                return;
            } catch (FileNotFoundException e) {
                activity.log_it("e", LOGTAG, "File Not Found Exception: " + e.toString());
                errorModalBox(getString(R.string.exception_RTLSDR_FILE_NOT_SAVED));
                return;
            } catch (IOException e) {
                activity.log_it("e", LOGTAG, "IO Exception: " + e.toString());
                errorModalBox(getString(R.string.exception_NO_NETWORK_ACCESS));
                return;
            }

            if (found == 1) {
                displayMessage(getString(R.string.msg_sending_data));

                // Instantiate the RequestQueue.
                RequestQueue queue = Volley.newRequestQueue(activity);
                String url = getFullUrl();
                activity.log_it("i", LOGTAG, "url is " + url);
                // Request a string response from the provided URL.
                StringRequest stringRequest = new StringRequest(StringRequest.Method.GET, url,
                        new Response.Listener<String>() {
                            @Override
                            public void onResponse(String response) {
                                displayMessage(getString(R.string.msg_data_rcvd_ok) + "\n\n-----------\n\n");
                            }
                        }, new Response.ErrorListener() {
                    @Override
                    public void onErrorResponse(VolleyError error) {
                        displayMessage(getString(R.string.msg_data_not_rcvd_ok) + "\n\n-----------\n\n");
                    }
                });
                // Add the request to the RequestQueue.
                queue.add(stringRequest);

            } else {
                displayMessage(getString(R.string.msg_valid_pkt_not_found));
            }

            Log.i(LOGTAG, "Exit postProcessRtlsdrData");
            Log.i(LOGTAG, "F @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()) + "\n");

            displayMessage("Completed at " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()) + "\n\n");
        }

        try {
            completeOk();
        } catch (InterruptedException e) {
        }
        return;
    }



    private void completeOk() throws InterruptedException {
        feedback(RtlSdrFeedbackTypes.EXTENDED_DATA_COMPLETEOK, "ok");
    }

    private void displayMessage(String str) {
        feedback(RtlSdrFeedbackTypes.EXTENDED_DATA_STATUS, str);
    }

    private void errorModalBox(String str) {
        feedback(RtlSdrFeedbackTypes.EXTENDED_DATA_FAILURE, str);
    }

    private void feedback(String feedbackType, String str) {
        // Puts the status into the Intent
        Intent localIntent = new Intent(RtlSdrFeedbackTypes.BROADCAST_ACTION).putExtra(feedbackType, str);
        // Broadcasts the Intent to receivers in this app.
        LocalBroadcastManager.getInstance(this).sendBroadcast(localIntent);
    }



    public double getKpa() {
        return Double.parseDouble(resp.split(",")[2].trim());
    }

    public double getPsi() {
        return (getKpa() * 0.145037738);
    }

    public long getTempF() {
        return (long) ((double) getTempC() * 1.8 + 32.0);
    }

    public long getTempC() {
        return Integer.parseInt(resp.split(",")[1].trim());
    }

    public long getDecAddr() {
        return Integer.parseInt(getHexAddr(), 16);
    }

    public String getHexAddr() {
        return resp.split(",")[0].trim();
    }

    public String getFullUrl() {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd'%20'HH:mm:ss", Locale.US);
        sdf.setTimeZone(TimeZone.getTimeZone("UTC"));
        return ("http://server11288.baremetalcloud.com/tire_samples/create?tire_sample[sensor_id]=" +
                getDecAddr() +
                "&tire_sample[receiver_id]=" +
                "8" +
                "&tire_sample[psi]=" +
                getPsi() + "" +
                "&tire_sample[tempc]=" +
                getTempC() +
                "&tire_sample[sample_time]=" +
                sdf.format(new Date()));
    }

}

