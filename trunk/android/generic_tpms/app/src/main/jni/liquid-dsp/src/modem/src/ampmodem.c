/*
 * Copyright (c) 2007 - 2014 Joseph Gaeddert
 *
 * This file is part of liquid.
 *
 * liquid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * liquid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with liquid.  If not, see <http://www.gnu.org/licenses/>.
 */

//
// Amplitude modulator/demodulator
//

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "liquid.internal.h"

#define DEBUG_AMPMODEM             0
#define DEBUG_AMPMODEM_FILENAME    "ampmodem_internal_debug.m"
#define DEBUG_AMPMODEM_BUFFER_LEN  (400)

#if DEBUG_AMPMODEM
void ampmodem_debug_print(ampmodem _q, const char * _filename);
#endif

struct ampmodem_s {
    float m;                    // modulation index
    liquid_ampmodem_type type;  // modulation type
    int suppressed_carrier;     // suppressed carrier flag
    float fc;                   // carrier frequency

    // demod objects
    nco_crcf oscillator;

    // suppressed carrier
    // TODO : replace DC bias removal with iir filter object
    float ssb_alpha;    // dc bias removal
    float ssb_q_hat;

    // single side-band
    firhilbf hilbert;   // hilbert transform

    // double side-band

    // debugging
#if DEBUG_AMPMODEM
    windowcf debug_x;
    windowf  debug_phase_error;
    windowf  debug_freq_error;
#endif
};

// create ampmodem object
//  _m                  :   modulation index
//  _fc                 :   carrier frequency
//  _type               :   AM type (e.g. LIQUID_AMPMODEM_DSB)
//  _suppressed_carrier :   carrier suppression flag
ampmodem ampmodem_create(float _m,
                         float _fc,
                         liquid_ampmodem_type _type,
                         int _suppressed_carrier)
{
    ampmodem q = (ampmodem) malloc(sizeof(struct ampmodem_s));
    q->type = _type;
    q->m    = _m;
    q->fc   = _fc;
    q->suppressed_carrier = (_suppressed_carrier == 0) ? 0 : 1;

    // create nco, pll objects
    q->oscillator = nco_crcf_create(LIQUID_NCO);
    nco_crcf_set_frequency(q->oscillator, 2*M_PI*q->fc);

    nco_crcf_pll_set_bandwidth(q->oscillator,0.05f);

    // suppressed carrier
    q->ssb_alpha = 0.01f;
    q->ssb_q_hat = 0.0f;

    // single side-band
    q->hilbert = firhilbf_create(9, 60.0f);

    // double side-band

    ampmodem_reset(q);

    // debugging
#if DEBUG_AMPMODEM
    q->debug_x =           windowcf_create(DEBUG_AMPMODEM_BUFFER_LEN);
    q->debug_phase_error =  windowf_create(DEBUG_AMPMODEM_BUFFER_LEN);
    q->debug_freq_error =   windowf_create(DEBUG_AMPMODEM_BUFFER_LEN);
#endif

    return q;
}

void ampmodem_destroy(ampmodem _q)
{
#if DEBUG_AMPMODEM
    // export output debugging file
    ampmodem_debug_print(_q, DEBUG_AMPMODEM_FILENAME);

    // destroy debugging objects
    windowcf_destroy(_q->debug_x);
    windowf_destroy(_q->debug_phase_error);
    windowf_destroy(_q->debug_freq_error);
#endif

    // destroy nco object
    nco_crcf_destroy(_q->oscillator);

    // destroy hilbert transform
    firhilbf_destroy(_q->hilbert);

    // free main object memory
    free(_q);
}

void ampmodem_print(ampmodem _q)
{
    LOGI("ampmodem:\n");
    LOGI("    type            :   ");
    switch (_q->type) {
    case LIQUID_AMPMODEM_DSB: LOGI("double side-band\n");         break;
    case LIQUID_AMPMODEM_USB: LOGI("single side-band (upper)\n"); break;
    case LIQUID_AMPMODEM_LSB: LOGI("single side-band (lower)\n"); break;
    default:                  LOGI("unknown\n");
    }
    LOGI("    supp. carrier   :   %s\n", _q->suppressed_carrier ? "yes" : "no");
    LOGI("    mod. index      :   %-8.4f\n", _q->m);
}

void ampmodem_reset(ampmodem _q)
{
    // single side-band
    _q->ssb_q_hat = 0.5f;
}

void ampmodem_modulate(ampmodem _q,
                       float _x,
                       float complex *_y)
{
    float complex x_hat = 0.0f;
    float complex y_hat;

    if (_q->type == LIQUID_AMPMODEM_DSB) {
        x_hat = _x;
    } else {
        // push through Hilbert transform
        // LIQUID_AMPMODEM_USB:
        // LIQUID_AMPMODEM_LSB: conjugate Hilbert transform output
        firhilbf_r2c_execute(_q->hilbert, _x, &x_hat);

        if (_q->type == LIQUID_AMPMODEM_LSB)
            x_hat = conjf(x_hat);
    }

    if (_q->suppressed_carrier)
        y_hat = x_hat;
    else
        y_hat = 0.5f*(x_hat + 1.0f);

    // mix up
    nco_crcf_mix_up(_q->oscillator, y_hat, _y);
    nco_crcf_step(_q->oscillator);
}

void ampmodem_demodulate(ampmodem _q,
                         float complex _y,
                         float *_x)
{
#if DEBUG_AMPMODEM
    windowcf_push(_q->debug_x, _y);
#endif

    if (_q->suppressed_carrier) {
        // coherent demodulation

        // mix signal down
        float complex y_hat;
        nco_crcf_mix_down(_q->oscillator, _y, &y_hat);

        // compute phase error
        float phase_error = tanhf( crealf(y_hat) * cimagf(y_hat) );
#if DEBUG_AMPMODEM
        // compute frequency error
        float nco_freq   = nco_crcf_get_frequency(_q->oscillator);
        float freq_error = nco_freq/(2*M_PI) - _q->fc/(2*M_PI);

        // retain phase and frequency errors
        windowf_push(_q->debug_phase_error, phase_error);
        windowf_push(_q->debug_freq_error, freq_error);
#endif

        // adjust nco, pll objects
        nco_crcf_pll_step(_q->oscillator, phase_error);

        // step NCO
        nco_crcf_step(_q->oscillator);

        // set output
        *_x = crealf(y_hat);
    } else {
        // non-coherent demodulation (peak detector)
        float t = cabsf(_y);

        // remove DC bias
        _q->ssb_q_hat = (    _q->ssb_alpha)*t +
                        (1 - _q->ssb_alpha)*_q->ssb_q_hat;
        *_x = 2.0f*(t - _q->ssb_q_hat);
    }
}

// export debugging file
void ampmodem_debug_print(ampmodem _q,
                          const char * _filename)
{
    FILE * fid = fopen(_filename,"w");
    if (!fid) {
        LOGE("error: ofdmframe_debug_print(), could not open '%s' for writing\n", _filename);
        return;
    }
    LOGI("%% %s : auto-generated file\n", DEBUG_AMPMODEM_FILENAME);
#if DEBUG_AMPMODEM
    LOGI("close all;\n");
    LOGI("clear all;\n");
    LOGI("n = %u;\n", DEBUG_AMPMODEM_BUFFER_LEN);
    unsigned int i;
    float complex * rc;
    float * r;

    // plot received signal
    LOGI("x = zeros(1,n);\n");
    windowcf_read(_q->debug_x, &rc);
    for (i=0; i<DEBUG_AMPMODEM_BUFFER_LEN; i++)
        LOGI("x(%4u) = %12.4e + j*%12.4e;\n", i+1, crealf(rc[i]), cimagf(rc[i]));
    LOGI("figure;\n");
    LOGI("plot(0:(n-1),real(x),0:(n-1),imag(x));\n");
    LOGI("xlabel('sample index');\n");
    LOGI("ylabel('received signal, x');\n");

    // plot phase/freq error
    LOGI("phase_error = zeros(1,n);\n");
    windowf_read(_q->debug_phase_error, &r);
    for (i=0; i<DEBUG_AMPMODEM_BUFFER_LEN; i++)
        LOGI("phase_error(%4u) = %12.4e;\n", i+1, r[i]);

    LOGI("freq_error = zeros(1,n);\n");
    windowf_read(_q->debug_freq_error, &r);
    for (i=0; i<DEBUG_AMPMODEM_BUFFER_LEN; i++)
        LOGI("freq_error(%4u) = %12.4e;\n", i+1, r[i]);

    LOGI("figure;\n");
    LOGI("subplot(2,1,1),\n");
    LOGI("  plot(0:(n-1),phase_error);\n");
    LOGI("  xlabel('sample index');\n");
    LOGI("  ylabel('phase error');\n");
    LOGI("subplot(2,1,2),\n");
    LOGI("  plot(0:(n-1),freq_error);\n");
    LOGI("  xlabel('sample index');\n");
    LOGI("  ylabel('freq error');\n");

#else
    LOGI("disp('no debugging info available');\n");
#endif

    fclose(fid);
    LOGI("ampmodem/debug: results written to '%s'\n", _filename);
}


