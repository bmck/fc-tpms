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
// gmskdem.c : Gauss minimum-shift keying modem
//

#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <stdlib.h>

#include "liquid.internal.h"

#define DEBUG_GMSKDEM           0
#define DEBUG_GMSKDEM_FILENAME  "gmskdem_internal_debug.m"
#define DEBUG_BUFFER_LEN        (1000)

#define GMSKDEM_USE_EQUALIZER   1

void gmskdem_debug_print(gmskdem _q,
                         const char * _filename);

struct gmskdem_s {
    unsigned int k;         // samples/symbol
    unsigned int m;         // symbol delay
    float BT;               // bandwidth/time product
    unsigned int h_len;     // filter length
    float * h;              // pulse shaping filter

    // filter object
#if GMSKDEM_USE_EQUALIZER
    eqlms_rrrf eq;          // receiver matched filter/equalizer
    float k_inv;            // 1 / k
#else
    firfilt_rrrf filter;    // receiver matched filter
#endif

    float complex x_prime;  // received signal state

    // demodulated symbols counter
    unsigned int num_symbols_demod;

#if DEBUG_GMSKDEM
    windowf  debug_mfout;
#endif
};

// create gmskdem object
//  _k      :   samples/symbol
//  _m      :   filter delay (symbols)
//  _BT     :   excess bandwidth factor
gmskdem gmskdem_create(unsigned int _k,
                       unsigned int _m,
                       float _BT)
{
    if (_k < 2) {
        LOGE("error: gmskdem_create(), samples/symbol must be at least 2\n");
        exit(1);
    } else if (_m < 1) {
        LOGE("error: gmskdem_create(), symbol delay must be at least 1\n");
        exit(1);
    } else if (_BT <= 0.0f || _BT >= 1.0f) {
        LOGE("error: gmskdem_create(), bandwidth/time product must be in (0,1)\n");
        exit(1);
    }

    // allocate memory for main object
    gmskdem q = (gmskdem)malloc(sizeof(struct gmskdem_s));

    // set properties
    q->k  = _k;
    q->m  = _m;
    q->BT = _BT;

    // allocate memory for filter taps
    q->h_len = 2*(q->k)*(q->m)+1;
    q->h = (float*) malloc(q->h_len * sizeof(float));

    // compute filter coefficients
    liquid_firdes_gmskrx(q->k, q->m, q->BT, 0.0f, q->h);

#if GMSKDEM_USE_EQUALIZER
    // receiver matched filter/equalizer
    q->eq = eqlms_rrrf_create_rnyquist(LIQUID_FIRFILT_GMSKRX,
                                       q->k,
                                       q->m,
                                       q->BT,
                                       0.0f);
    eqlms_rrrf_set_bw(q->eq, 0.01f);
    q->k_inv = 1.0f / (float)(q->k);
#else
    // create filter object
    q->filter = firfilt_rrrf_create(q->h, q->h_len);
#endif

    // reset modem state
    gmskdem_reset(q);

#if DEBUG_GMSKDEM
    q->debug_mfout  = windowf_create(DEBUG_BUFFER_LEN);
#endif

    // return modem object
    return q;
}

void gmskdem_destroy(gmskdem _q)
{
#if DEBUG_GMSKDEM
    // print to external file
    gmskdem_debug_print(_q, DEBUG_GMSKDEM_FILENAME);

    // destroy debugging objects
    windowf_destroy(_q->debug_mfout);
#endif

    // destroy filter object
#if GMSKDEM_USE_EQUALIZER
    eqlms_rrrf_destroy(_q->eq);
#else
    firfilt_rrrf_destroy(_q->filter);
#endif

    // free filter array
    free(_q->h);

    // free main object memory
    free(_q);
}

void gmskdem_print(gmskdem _q)
{
    LOGI("gmskdem [k=%u, m=%u, BT=%8.3f]\n", _q->k, _q->m, _q->BT);
#if GMSKDEM_USE_EQUALIZER
    LOGI("    equalizer bandwidth :   %12.8f\n", eqlms_rrrf_get_bw(_q->eq));
#endif
    unsigned int i;
    for (i=0; i<_q->h_len; i++)
        LOGI("  hr(%4u) = %12.8f;\n", i+1, _q->h[i]);
}

void gmskdem_reset(gmskdem _q)
{
    // reset phase state
    _q->x_prime = 0.0f;

    // set demod. counter to zero
    _q->num_symbols_demod = 0;

    // clear filter buffer
#if GMSKDEM_USE_EQUALIZER
    eqlms_rrrf_reset(_q->eq);
#else
    firfilt_rrrf_reset(_q->filter);
#endif
}

// set equalizer bandwidth
void gmskdem_set_eq_bw(gmskdem _q,
                       float _bw)
{
    // validate input
    if (_bw < 0.0f || _bw > 0.5f) {
        LOGE("error: gmskdem_set_eq_bw(), bandwith must be in [0,0.5]\n");
        exit(1);
    }

#if GMSKDEM_USE_EQUALIZER
    // set internal equalizer bandwidth
    eqlms_rrrf_set_bw(_q->eq, _bw);
#else
    LOGE("warning: gmskdem_set_eq_bw(), equalizer is disabled\n");
#endif
}

void gmskdem_demodulate(gmskdem _q,
                        float complex * _x,
                        unsigned int * _s)
{
    // increment symbol counter
    _q->num_symbols_demod++;

    // run matched filter
    unsigned int i;
    float phi;
    float d_hat;
    for (i=0; i<_q->k; i++) {
        // compute phase difference
        phi = cargf( conjf(_q->x_prime)*_x[i] );
        _q->x_prime = _x[i];

        // run through matched filter
#if GMSKDEM_USE_EQUALIZER
        eqlms_rrrf_push(_q->eq, phi);
#else
        firfilt_rrrf_push(_q->filter, phi);
#endif

#if DEBUG_GMSKDEM
        // compute output
        float d_tmp;
#  if GMSKDEM_USE_EQUALIZER
        eqlms_rrrf_execute(_q->eq, &d_tmp);
#  else
        firfilt_rrrf_execute(_q->filter, &d_tmp);
#  endif
        windowf_push(_q->debug_mfout, d_tmp);
#endif

        // decimate by k
        if ( i != 0 ) continue;

#if GMSKDEM_USE_EQUALIZER
        // compute filter/equalizer output
        eqlms_rrrf_execute(_q->eq, &d_hat);
#else
        // compute filter output
        firfilt_rrrf_execute(_q->filter, &d_hat);
#endif
    }

    // make decision
    *_s = d_hat > 0.0f ? 1 : 0;

#if GMSKDEM_USE_EQUALIZER
    // update equalizer, after appropriate delay
    if (_q->num_symbols_demod >= 2*_q->m) {
        // compute expected output, scaling by samples/symbol
        float d_prime = d_hat > 0 ? _q->k_inv : -_q->k_inv;
        eqlms_rrrf_step(_q->eq, d_prime, d_hat);
    }
#endif
}

//
// output debugging file
//
void gmskdem_debug_print(gmskdem _q,
                         const char * _filename)
{
    // open output filen for writing
    FILE * fid = fopen(_filename,"w");
    if (!fid) {
        LOGE("error: gmskdem_debug_print(), could not open '%s' for writing\n", _filename);
        exit(1);
    }
    LOGI("%% %s : auto-generated file\n", _filename);
    LOGI("clear all\n");
    LOGI("close all\n");

#if DEBUG_GMSKDEM
    //
    unsigned int i;
    float * r;
    LOGI("n = %u;\n", DEBUG_BUFFER_LEN);
    LOGI("k = %u;\n", _q->k);
    LOGI("m = %u;\n", _q->m);
    LOGI("t = [0:(n-1)]/k;\n");

    // plot receive filter response
    LOGI("ht = zeros(1,2*k*m+1);\n");
    float ht[_q->h_len];
    liquid_firdes_gmsktx(_q->k, _q->m, _q->BT, 0.0f, ht);
    for (i=0; i<_q->h_len; i++)
        LOGI("ht(%4u) = %12.4e;\n", i+1, ht[i]);
#if GMSKDEM_USE_EQUALIZER
    float hr[_q->h_len];
    eqlms_rrrf_get_weights(_q->eq, hr);
    for (i=0; i<_q->h_len; i++)
        LOGI("hr(%4u) = %12.4e * %u;\n", i+1, hr[i], _q->k);
#else
    for (i=0; i<_q->h_len; i++)
        LOGI("hr(%4u) = %12.4e;\n", i+1, _q->h[i]);
#endif
    LOGI("hc = conv(ht,hr)/k;\n");
    LOGI("nfft = 1024;\n");
    LOGI("f = [0:(nfft-1)]/nfft - 0.5;\n");
    LOGI("Ht = 20*log10(abs(fftshift(fft(ht/k, nfft))));\n");
    LOGI("Hr = 20*log10(abs(fftshift(fft(hr/k, nfft))));\n");
    LOGI("Hc = 20*log10(abs(fftshift(fft(hc/k, nfft))));\n");
    LOGI("figure;\n");
    LOGI("plot(f,Ht, f,Hr, f,Hc,'-k','LineWidth',2);\n");
    LOGI("axis([-0.5 0.5 -50 10]);\n");
    LOGI("xlabel('Normalized Frequency');\n");
    LOGI("ylabel('Power Spectral Density [dB]');\n");
    LOGI("legend('transmit','receive','composite',1);\n");
    LOGI("grid on;\n");

    LOGI("mfout = zeros(1,n);\n");
    windowf_read(_q->debug_mfout, &r);
    for (i=0; i<DEBUG_BUFFER_LEN; i++)
        LOGI("mfout(%5u) = %12.4e;\n", i+1, r[i]);
    LOGI("i0 = 1; %%mod(k+n,k)+k;\n");
    LOGI("isym = i0:k:n;\n");
    LOGI("figure;\n");
    LOGI("plot(t,mfout,'-', t(isym),mfout(isym),'o','MarkerSize',4);\n");
    LOGI("grid on;\n");
#endif

    fclose(fid);
    LOGI("gmskdem: internal debugging written to '%s'\n", _filename);
}
