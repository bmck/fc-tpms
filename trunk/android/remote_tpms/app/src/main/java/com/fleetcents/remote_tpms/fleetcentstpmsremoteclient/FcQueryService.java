package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.app.IntentService;
import android.content.Context;
import android.content.Intent;
import android.content.SharedPreferences;
import android.net.Uri;
import android.preference.PreferenceManager;
import android.support.v4.content.LocalBroadcastManager;
import android.util.Log;

import com.android.volley.AuthFailureError;
import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.RequestFuture;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import org.acra.ACRA;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.HashMap;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.TimeoutException;

/**
 * Created by bmck on 12/5/15.
 */
public class FcQueryService extends IntentService {

    private static final String LOGTAG = "FcQueryService";

    private static RequestQueue queue;
    private static SharedPreferences sharedPrefs;

    private static final String serverProtocol = "http";
    private static final String serverHostname = "www.fleetcents.com";

    public static final String loginUri = "/user/sign_in.json";
    public static final String loginParms = "";
    public static final int loginMethod = StringRequest.Method.POST;

    private static final String logoutUri = "/user/sign_out.json";
    private static final String logoutParms = "";
    private static final int logoutMethod = StringRequest.Method.DELETE;

    static void init(Context context) {
        queue = Volley.newRequestQueue(context);
        sharedPrefs = PreferenceManager.getDefaultSharedPreferences(context);
    }

    public FcQueryService() {
        super("FcQueryService");
    }
    public FcQueryService(String name) { super(name); }

    @Override
    protected void onHandleIntent(Intent workIntent) {
        // Gets data from the incoming Intent
        String str = workIntent.getDataString();
        Log.i(LOGTAG, "str = " + str);
        Uri u = Uri.parse(str);

        if (!callToServer(u)) { ACRA.getErrorReporter().handleSilentException(null); }
    }

    protected boolean callToServer(Uri u) {
        if (!AbstractBaseActivity.apiTokenAvailable()) {
            if (loginToServer() == false) { return false; }
        }

        String method = u.getQueryParameter("meth");
        int meth = Request.Method.GET;

        if (method.equals("DELETE")) { return logoutOfServer(); }
        else if (method.equals("POST")) { meth = Request.Method.POST; }
//        else if (method.equals("PATCH")) { meth = Request.Method.PATCH; }

        try {
            fetchUrl(getFullUrl(u.getPath(), u.getQuery()), meth);
        } catch (JSONException e) {
            Log.i(LOGTAG, "Error A\n");
            handleServerError(e);
            return logoutOfServer();
        }
        return true;
    }

    private void fetchUrl(final String url, int methodType) throws JSONException {
        Log.i(LOGTAG, "fetching " + url + "\n");
        // Request a string response from the provided URL.
        StringRequest myReq = new StringRequest(methodType, url,
                new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        Log.i(LOGTAG, "response = >" + response + "<\n");
                        try {
                            JSONObject jsonObj = (response.length() > 0 ? new JSONObject(response) : new JSONObject());
                            Uri u = Uri.parse(url);
                            String prms = u.getQuery();
                            if (prms == null) { prms = ""; }
                            jsonObj.put("url", (u.getPath() + (prms.length() > 0 ? ("?" + prms) : "")));
                            handleServerResponse(jsonObj);
                        } catch (JSONException e) {
                            Log.i(LOGTAG, "Error B\n");
                            handleServerError(e);
                        }
                    }
                },
                createMyReqErrorListener()
        ) {
            @Override
            public HashMap<String, String> getHeaders() throws AuthFailureError {
                HashMap<String, String> hdrs = new HashMap<String, String>();
                hdrs.put("Content-Type", "application/json");
                if (sharedPrefs.getString("userid", "").length() > 0) {
                    hdrs.put("X-USER-EMAIL", sharedPrefs.getString("userid", ""));
                }
                if (AbstractBaseActivity.apiTokenAvailable()) {
                    hdrs.put("X-USER-TOKEN", AbstractBaseActivity.apiToken());
                }
                Log.i(LOGTAG, "hdrs = " + hdrs.toString() + "\n");
                return hdrs;
            }

        };
        // Add the request to the RequestQueue.
        queue.add(myReq);
    }

    public boolean loginToServer() {
        Response.ErrorListener errorListener = createMyReqErrorListener();
        try {
            int method = loginMethod;
            String url = getFullUrl(loginUri, loginParms);
            Log.i(LOGTAG, "logging in to server at url = " + url + "\n");

            // set-up call parameters
            JSONObject subparams = new JSONObject();
            subparams.put("email", sharedPrefs.getString("userid", ""));
            subparams.put("password", sharedPrefs.getString("password", ""));
            JSONObject params = new JSONObject();
            params.put("user", subparams);
            Log.i(LOGTAG, "params = " + params.toString() + "\n");

            // issue blocking request to queue
            RequestFuture<JSONObject> future = RequestFuture.newFuture();
            JsonObjectRequest request = new JsonObjectRequest(method, url, params, future, errorListener);
            queue.add(request);

            // set timeout to 2 sec
            JSONObject response = future.get(2, TimeUnit.SECONDS);
            AbstractBaseActivity.setApiToken(response.getString("authentication_token"));
            return true;
        } catch (InterruptedException e) {
            Log.e(LOGTAG, "Retrieve cards api call interrupted.  " + e);
            errorListener.onErrorResponse(new VolleyError(e));
        } catch (ExecutionException e) {
            Log.e(LOGTAG, "Retrieve cards api call failed.  " + e);
            errorListener.onErrorResponse(new VolleyError(e));
        } catch (TimeoutException e) {
            Log.e(LOGTAG, "Retrieve cards api call timed out.  " + e);
            errorListener.onErrorResponse(new VolleyError(e));
        } catch (JSONException e) {
            // TBD
        }
        return false;
    }

    protected boolean logoutOfServer() {
        try {
            AbstractBaseActivity.setApiToken("");
            fetchUrl(getFullUrl(logoutUri, logoutParms), logoutMethod);
            return true;
        } catch (JSONException e) {
            Log.i(LOGTAG, "Error C\n");
            handleServerError(e);
        }
        return true;
    }

    protected void handleServerResponse(JSONObject jsonObj) {
        Log.i(LOGTAG, "calling handleServerResponse\n");
        Intent resultIntent = null;
        try {
            String u = jsonObj.getString("url").toString();
            resultIntent = new Intent(u);
            Log.i(LOGTAG, "calling handleServerResponse w resultIntent = " + u + "\n");
            resultIntent.putExtra("response", jsonObj.toString());
            Log.i(LOGTAG, "resultIntent = " + resultIntent.toString() + "\n");
            LocalBroadcastManager.getInstance(this).sendBroadcast(resultIntent);
        } catch (JSONException e) {
            Log.i(LOGTAG, "Error D\n");
            e.printStackTrace();
        }
    }

    private Response.ErrorListener createMyReqErrorListener() {
        return new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError err) {
                Log.i(LOGTAG, "Error E\n");
                handleServerError(err);
            }
        };
    }

    protected void handleServerError(Exception err) {
        try {
            AbstractBaseActivity.setApiToken("");
            JSONObject faux_response = new JSONObject();
            String e = err.toString();
            faux_response.put("error", e);
            Log.i(LOGTAG, "error = " + faux_response.toString() + "\n");
        } catch (JSONException e1) {
            Log.i(LOGTAG, "Error F\n");
            Log.i(LOGTAG, "error = JSONException and something else\n");
        }
        ACRA.getErrorReporter().handleSilentException(null);
    }

    protected String getFullUrl(String uri, String parms) {
        return (serverProtocol + "://" + serverHostname) + (uri + (parms.length() > 0 ? "?" + parms : ""));
    }

}
