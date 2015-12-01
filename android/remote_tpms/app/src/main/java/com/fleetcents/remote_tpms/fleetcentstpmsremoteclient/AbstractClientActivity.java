package com.fleetcents.remote_tpms.fleetcentstpmsremoteclient;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler.Callback;
import android.os.Message;

import com.android.volley.Request;
import com.android.volley.RequestQueue;
import com.android.volley.Response;
import com.android.volley.VolleyError;
import com.android.volley.toolbox.JsonObjectRequest;
import com.android.volley.toolbox.StringRequest;
import com.android.volley.toolbox.Volley;

import org.acra.ACRA;
import org.json.JSONException;
import org.json.JSONObject;

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.CharArrayReader;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Properties;

/**
 * Created by bmck on 11/25/15.
 */
public class AbstractClientActivity extends AbstractBaseActivity {

    String apiToken = "";
    private RequestQueue queue = Volley.newRequestQueue(this);

    protected void handleServerResponse(JSONObject jsonObj) {
        ACRA.getErrorReporter().handleSilentException(null);
    }

//    protected void checkApiToken() {
//        if (apiToken.equals("")) {
//            loginToServer();
//        }
//    }

    private boolean loginToServer() {
        try {
            String url = getFullUrl(loginUri(), loginParms());
            int methodType = loginMethod();

            JSONObject subparams = new JSONObject();
            subparams.put("email", getString(R.string.userid));
            subparams.put("password", getString(R.string.password));
            JSONObject params = new JSONObject();
            params.put("user", subparams);

            JsonObjectRequest myReq = new JsonObjectRequest(methodType, url,
                    params,
                    createMyReqLoginSuccessListener(),
                    createMyReqErrorListener()
            );

            queue.add(myReq);

            return true;
        } catch (JSONException e) {
            ACRA.getErrorReporter().handleSilentException(null);
            try {
                JSONObject faux_response = new JSONObject();
                String err = e.toString();
                faux_response.put("error", e);
                handleServerResponse(faux_response);
            } catch (JSONException e1) {
                return false;
            }
            logoutOfServer();
        }
        return true;
    }

    protected boolean logoutOfServer() {
        try {
            fetchUrl(getFullUrl(logoutUri(), logoutParms()), logoutMethod());
            return true;
        } catch (JSONException e) {
            ACRA.getErrorReporter().handleSilentException(null);
            try {
                JSONObject faux_response = new JSONObject();
                String err = e.toString();
                faux_response.put("error", err);
                handleServerResponse(faux_response);
            } catch (JSONException e1) {
                return false;
            }
        }
        return true;
    }

    protected boolean callToServer(String uri, String parms, String method) {
        if (apiToken.equals("")) {
            if (loginToServer() == false)
                return false;
        }

        int meth = Request.Method.GET;
        if (method.equals("DELETE")) {
            meth = Request.Method.DELETE;
        } else if (method.equals("POST")) {
            meth = Request.Method.POST;
        } else if (method.equals("PATCH")) {
            meth = Request.Method.PATCH;
        }

        try {
            fetchUrl(getFullUrl(uri, parms), meth);
            return true;
        } catch (JSONException e) {
            ACRA.getErrorReporter().handleSilentException(null);
            try {
                JSONObject faux_response = new JSONObject();
                String err = e.toString();
                faux_response.put("error", err);
                handleServerResponse(faux_response);
            } catch (JSONException e1) {
                return false;
            }
            logoutOfServer();
        }
        return true;
    }


    private void fetchUrl(String url, int methodType) throws JSONException {
        // Request a string response from the provided URL.
        StringRequest myReq = new StringRequest(methodType, url,
                new Response.Listener<String>() {
                    @Override
                    public void onResponse(String response) {
                        try {
                            JSONObject jsObj = new JSONObject(response);
                        } catch (JSONException e) {
                            try {
                                JSONObject faux_response = new JSONObject();
                                String err = e.toString();
                                faux_response.put("error", err);
                                handleServerResponse(faux_response);
                            } catch (JSONException e1) {
                            }
                        }
                    }
                },
                createMyReqErrorListener()
        ) {
            @Override
            public HashMap<String, String> getParams() {
                HashMap<String, String> params = new HashMap<String, String>();
                params.put("Content-Type", "application/json");
                if (getString(R.string.userid).length() > 0) {
                    params.put("X-USER-EMAIL", getString(R.string.userid));
                }
                if (apiToken.length() > 0) {
                    params.put("X-USER-TOKEN", apiToken);
                }
                return params;
            }

        };
        // Add the request to the RequestQueue.
        queue.add(myReq);
    }

    private Response.Listener<JSONObject> createMyReqLoginSuccessListener() {
        return new Response.Listener<JSONObject>() {
            @Override
            public void onResponse(JSONObject response) {
                try {
                    apiToken = response.getString("authentication_token");
                    handleServerResponse(response);
                } catch (JSONException err) {
                    ACRA.getErrorReporter().handleSilentException(null);
                    try {
                        JSONObject faux_response = new JSONObject();
                        String e = err.toString();
                        faux_response.put("error", e);
                        handleServerResponse(faux_response);
                    } catch (JSONException e1) {
                    }
                }
            }
        };
    }

    private Response.Listener<JSONObject> createMyReqSuccessListener() {
        return new Response.Listener<JSONObject>() {
            @Override
            public void onResponse(JSONObject response) {
                    handleServerResponse(response);
            }
        };
    }

    private Response.ErrorListener createMyReqErrorListener() {
        return new Response.ErrorListener() {
            @Override
            public void onErrorResponse(VolleyError err) {
                ACRA.getErrorReporter().handleSilentException(null);
                try {
                    JSONObject faux_response = new JSONObject();
                    String e = err.toString();
                    faux_response.put("error", e);
                    handleServerResponse(faux_response);
                } catch (JSONException e1) {
                }
            }
        };
    }

    protected String getFullUrl(String uri, String parms) {
        return (serverProtocol() + "://" + serverHostname()) + ("/" + uri + (parms.length() > 0 ? "?" + parms : ""));
    }

    private String serverHostname() {
        return "server11288.baremetalcloud.com";
    }

    private String serverProtocol() {
        return "https";
    }


    private String loginUri() {
        return "user/sign_in.json";
    }

    private String loginParms() {
        return "";
    }

    private int loginMethod() {
        return StringRequest.Method.GET;
    }


    private String logoutUri() {
        return "user/sign_out";
    }

    private String logoutParms() {
        return "";
    }

    private int logoutMethod() {
        return StringRequest.Method.DELETE;
    }

}
