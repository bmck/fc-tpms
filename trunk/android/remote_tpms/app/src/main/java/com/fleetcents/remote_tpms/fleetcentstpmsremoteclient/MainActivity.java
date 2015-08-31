package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.preference.PreferenceManager;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;
import android.widget.TextView;

import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;
import com.fleetcents.remote_tpms.fleetcentstpmsremoteclient.core.RtlSdr;
import com.fleetcents.remote_tpms.fleetcentstpmsremoteclient.core.RtlSdrException;
import com.fleetcents.remote_tpms.fleetcentstpmsremoteclient.core.RtlSdrStartException;
import com.fleetcents.remote_tpms.fleetcentstpmsremoteclient.core.UsbHelper;

import org.apache.commons.net.ftp.FTP;
import org.apache.commons.net.ftp.FTPClient;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.TimeZone;

public class MainActivity extends Activity {

    private static final String LOGTAG = "MainActivity";
    private static final String TAG = "FleetCents";
    private static final String TMPFCBINFN = "fc.bin";

    private MenuItem mi_startStop = null;
    private TextView myText = null;
    private boolean running = false;
    private boolean testing = false;

    private String arguments = "";
    private String uspfs_path = null;
    private int fd = -1;

    private UsbManager mUsbManager = null;
    public static PendingIntent permissionIntent;
    public static Intent intent;

    public MainActivity activity = this;

    // TODO: Add client diagnostic data
    // TODO: Add client error modal dialog boxes
    private class WorkerThread extends Thread {
        public String resp;

        @Override
        public void run() {
            synchronized (this) {
                Log.i(LOGTAG, "A @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()) + "\n");
                displayMessage(getString(R.string.msg_send_activation) + " at " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));

                File file = new File(cacheDir(TMPFCBINFN), TMPFCBINFN);
                int sample_rate = 2048000;
                SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(activity);
                double secs = Double.parseDouble(sharedPrefs.getString("basestation_secssent", "1.0"));
                arguments = "-f 314980000 -s " + sample_rate + " -n " + Math.round(secs * sample_rate) + " -t " + (testing ? "1" : "0") + " " + file + "";
                Log.i(LOGTAG, "arguments = >" + arguments + "<");

                if (!testing) {
                    final UsbDevice device = UsbHelper.findDevice(activity);
                    final UsbDeviceConnection connection = UsbHelper.openDevice(activity, device);
                    if (connection == null) {
                        throw new RuntimeException(String.valueOf(R.string.exception_FAILED_TO_OPEN_DEVICE));
                    }

                    displayMessage(getString(R.string.msg_wait_for_sensor_data));

                    Log.d(LOGTAG, "B @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                    RtlSdr.open(arguments, connection.getFileDescriptor(), UsbHelper.properDeviceName(device.getDeviceName()));
                    Log.d(LOGTAG, "C @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                } else {
                    displayMessage(getString(R.string.msg_wait_for_sensor_data));

                    Log.d(LOGTAG, "B @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                    RtlSdr.open(arguments, -1, null);
                    Log.d(LOGTAG, "C @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));

                }
                displayMessage(getString(R.string.msg_rcving_sensor_data) + " at " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()));
                Log.i(LOGTAG, "onActivityResult: RTL-SDR Data Capture Completed.");

                if (pktFound(file) == 1) {
                    displayMessage(getString(R.string.msg_sending_data));

                    // Instantiate the RequestQueue.
                    RequestQueue queue = Volley.newRequestQueue(activity);
                    String url = getFullUrl();
                    Log.i(LOGTAG, "url is >" + url + "<");
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

                running = false;
                updateActionBarAnimation();
                Log.i(LOGTAG, "G @ " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()) + "\n");

                this.notify();
                displayMessage("Completed at " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()) + "\n\n");

            }
        }

        public int pktFound(File file) {
            SharedPreferences sharedPrefs = PreferenceManager.getDefaultSharedPreferences(activity);
            String base = sharedPrefs.getString("basestation_hostname", "");
            Log.i(LOGTAG, "base=" + base);

            FTPClient ftpClient = new FTPClient();
            resp = ",,,";
            int found = 0;
            try {
                String addr = InetAddress.getByName(base).getHostAddress();
                Log.i(LOGTAG, "connecting to " + addr);
                ftpClient.connect(addr);
                // TODO: use password field of login to identify user or hardware being used
                ftpClient.enterLocalPassiveMode();
                ftpClient.login("anonymous", "guest");
                Log.i(LOGTAG, ftpClient.getReplyString());

                ftpClient.changeWorkingDirectory("fleet_server");
                ftpClient.setFileType(FTP.BINARY_FILE_TYPE);
                BufferedInputStream buffIn = null;
                buffIn = new BufferedInputStream(new FileInputStream(file));
                ftpClient.enterLocalPassiveMode();
                ftpClient.storeUniqueFile(buffIn);
                buffIn.close();
                resp = ftpClient.getReplyString().split(" ")[1];
                Log.i(LOGTAG, "resp = " + resp);
                Log.i(LOGTAG, "resp[0] = >" + resp.split(",")[0] + "<");
                Log.i(LOGTAG, "resp[1] = >" + resp.split(",")[1] + "<");
                Log.i(LOGTAG, "resp[2] = >" + resp.split(",")[2] + "<");
                // Log.i(LOGTAG, "resp[3] = >" + resp.split(",")[3].trim() + "<");

                try {
                    displayMessage("\n" +
                            new SimpleDateFormat("yyyy-MM-dd HH:mm:ss.SSS").format(new Date()) + "\n" +
                            getString(R.string.sensor_id) + " " + getHexAddr() + "\n" +
                            getString(R.string.temp) + " " + getTempC() + " " + getString(R.string.celsius) + "\n" +
                            getString(R.string.pressure) + " " + getPsi() + " " + getString(R.string.psi) + "\n");
                    found = 1;
                } catch (NumberFormatException e) {
                    found = 0;
                }

                // resp code is standard FTP (unchanged) indicating success/failure
                // resp format is CSV: hex address, tempC, pressure_kpa, 0/1_successful_upload
                ftpClient.logout();
                ftpClient.disconnect();
            } catch (UnknownHostException e) {
                e.printStackTrace();
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
            return found;
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

    static {
        try {
            Log.i(LOGTAG, "Loading libraries");
            System.loadLibrary("crystax");
            System.loadLibrary("rtlsdr");
            Log.i(LOGTAG, "Successfully loaded native libraries");
        } catch (Throwable t) {
            Log.w(LOGTAG, "Failed to load native libraries");
        }
    }

    public String cacheDir(String fn) {
        // emulator expects dir at /storage/sdcard/Android/data/com.fleetcents.remote_tpms.fleetcentstpmsremoteclient/cache
        File[] dirs = ContextCompat.getExternalFilesDirs(this, "rtlsdr");
        File fdir = dirs[dirs.length - 1];
        String retVal = String.valueOf(fdir);
//       setCacheDir(retVal);
        if (!testing) {
            File f = new File(fdir, fn);
            f.mkdirs();
            f.delete();
        }
        return retVal;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        Log.i(LOGTAG, "Enter onCreate");
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        Log.i(LOGTAG, "whats wrong with following line?");
        setContentView(R.layout.activity_main);

        myText = (TextView) findViewById(R.id.main_messages);
        myText.setInputType(0x00020001);
        myText.setText(getString(R.string.hello_world) + "\n");

        running = false;

        Log.i(LOGTAG, "Exit onCreate");
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        Log.i(LOGTAG, "Enter onCreateOptionsMenu");

        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main_menu, menu);
        // Get a reference to the start-stop button:
        mi_startStop = menu.findItem(R.id.action_startstop);
        mi_startStop.setActionView(null);

        permissionIntent = PendingIntent.getBroadcast(this, 0, new Intent("com.android.example.USB_PERMISSION"), 0);
        mUsbManager = (UsbManager) this.getSystemService(Context.USB_SERVICE);

        Log.i(LOGTAG, "Exit onCreateOptionsMenu");
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        Log.i(LOGTAG, "Enter onOptionsItemSelected");
        // Handle presses on the action bar items
        switch (item.getItemId()) {
            case R.id.action_settings:
                enterSettings(findViewById(R.id.action_settings));
                return true;
            case R.id.action_startstop:
                if (!running) {
                    running = true;
                    updateActionBarAnimation();
                    WorkerThread t = new WorkerThread();
                    synchronized (t) {
                        t.start();
                        try {
                            t.wait();
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                    }
                }
                Log.i(LOGTAG, "Exit onOptionsItemSelected");
                return true;
            default:
                Log.i(LOGTAG, "Exit onOptionsItemSelected");
                return super.onOptionsItemSelected(item);
        }
    }

    private void updateActionBarAnimation() {
        Log.i(LOGTAG, "Enter updateActionBarAnimation");
        // Set icon of the start/stop button according to the state:
        if (mi_startStop != null) {
            if (running) {
                LayoutInflater inflater = (LayoutInflater) this.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                ImageView imageView = (ImageView) inflater.inflate(R.layout.iv_refresh, null);
                Animation fade = AnimationUtils.loadAnimation(this, R.anim.fade);
                imageView.startAnimation(fade);
                mi_startStop.setActionView(R.layout.iv_refresh);
//                MenuItemCompat.setActionView(mi_startStop, imageView);
            } else {
                mi_startStop.setActionView(null);
            }
        }
        Log.i(LOGTAG, "Exit updateActionBarAnimation");
    }

    public void enterSettings(View view) {
        Intent intent = new Intent(this, SettingsActivity.class);
        startActivity(intent);
    }

    private void displayMessage(final String str) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                myText.append(str + "\n");
                Log.d(LOGTAG, "displayed >" + str + "<?\n");
            }
        });
        Log.d(LOGTAG, "display >" + str + "<?\n");
    }


    /**
     * Announce there is an error. The id of the error could be get with RtlTcpExceptionId
     * from the parent
     */
    public void finishWithError(int id, Integer second_id, String msg) {
        final Intent data = new Intent();
        data.putExtra("com.fleetcents.remote_tpms.fleetcentstpmsremoteclient.RtlTcpExceptionId", id);

        if (second_id != null) data.putExtra("detailed_exception_code", second_id);
        if (msg != null) data.putExtra("detailed_exception_message", msg);

        if (getParent() == null) {
            setResult(RESULT_CANCELED, data);
        } else {
            getParent().setResult(RESULT_CANCELED, data);
        }
        finish();
    }

    public void finishWithError(final Exception e) {
        if (e == null) {
            finishWithError();
            return;
        }
        if ((e instanceof RtlSdrStartException))
            finishWithError(((RtlSdrStartException) e).getReason());
        else if (e instanceof RtlSdrException) {
            final RtlSdrException rtlexception = (RtlSdrException) e;
            finishWithError(rtlexception.getReason(), rtlexception.id, rtlexception.getMessage());
        } else
            finishWithError();
    }

    public void finishWithError(final RtlSdrStartException.err_info info) {
        finishWithError(info, null, null);
    }

    public void finishWithError(final RtlSdrStartException.err_info info, Integer second_id, String msg) {
        if (info != null)
            finishWithError(info.ordinal(), second_id, msg);
        else
            finishWithError(second_id, msg);
    }

    public void finishWithError() {
        finishWithError(null, null);
    }

    public void finishWithError(Integer second_id, String msg) {
        finishWithError(-1, second_id, msg);
    }

    private void finishWithSuccess() {
        final Intent data = new Intent();

        if (getParent() == null) {
            setResult(RESULT_OK, data);
        } else {
            getParent().setResult(RESULT_OK, data);
        }
        finish();
    }

}