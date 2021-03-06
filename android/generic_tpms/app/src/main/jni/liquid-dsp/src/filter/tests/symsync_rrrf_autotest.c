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
// symsync_rrrf_autotest.c : test symbol timing synchronizer
//

#include "autotest/autotest.h"
#include "liquid.h"

//
void symsync_rrrf_test(unsigned int _k,
                       unsigned int _m,
                       float        _beta,
                       float        _tau,
                       float        _rate)
{
    // options
    float        tol        =  0.2f;    // error tolerance
    unsigned int k          =  _k;      // samples/symbol (input)
    unsigned int m          =  _m;      // filter delay (symbols)
    float        beta       =  _beta;   // filter excess bandwidth factor
    unsigned int num_filters= 32;       // number of filters in the bank

    unsigned int num_symbols_init=200;  // number of initial symbols
    unsigned int num_symbols_test=100;  // number of testing symbols

    // transmit/receive filter types
    liquid_firfilt_type ftype_tx = LIQUID_FIRFILT_ARKAISER;
    liquid_firfilt_type ftype_rx = LIQUID_FIRFILT_ARKAISER;

    float bt    =  0.02f;               // loop filter bandwidth
    float tau   =  _tau;                // fractional symbol offset
    float rate  =  _rate;               // resampled rate

    // derived values
    unsigned int num_symbols = num_symbols_init + num_symbols_test;
    unsigned int num_samples = k*num_symbols;
    unsigned int num_samples_resamp = (unsigned int) ceilf(num_samples*rate*1.1f) + 4;

    // compute delay
    while (tau < 0) tau += 1.0f;    // ensure positive tau
    float g = k*tau;                // number of samples offset
    int ds=floorf(g);               // additional symbol delay
    float dt = (g - (float)ds);     // fractional sample offset
    if (dt > 0.5f) {                // force dt to be in [0.5,0.5]
        dt -= 1.0f;
        ds++;
    }

    unsigned int i;

    // allocate arrays
    float s[num_symbols];           // data symbols
    float x[num_samples];           // interpolated samples
    float y[num_samples_resamp];    // resampled data (resamp_rrrf)
    float z[num_symbols + 64];      // synchronized symbols

    // generate pseudo-random BPSK symbols
    // NOTE: by using an m-sequence generator this sequence will be identical
    //       each time this test is run
    msequence ms = msequence_create_default(10);
    for (i=0; i<num_symbols; i++)
        s[i] = (msequence_generate_symbol(ms,1) ? -1.0f : 1.0f);
    msequence_destroy(ms);

    //
    // create and run interpolator
    //

    // design interpolating filter
    firinterp_rrrf interp = firinterp_rrrf_create_rnyquist(ftype_tx,k,m,beta,dt);

    // interpolate block of samples
    firinterp_rrrf_execute_block(interp, s, num_symbols, x);

    // destroy interpolator
    firinterp_rrrf_destroy(interp);

    //
    // run resampler
    //

    // create resampler
    unsigned int resamp_len = 10*k; // resampling filter semi-length (filter delay)
    float resamp_bw = 0.45f;        // resampling filter bandwidth
    float resamp_As = 60.0f;        // resampling filter stop-band attenuation
    unsigned int resamp_npfb = 64;  // number of filters in bank
    resamp_rrrf resamp = resamp_rrrf_create(rate, resamp_len, resamp_bw, resamp_As, resamp_npfb);

    // run resampler on block
    unsigned int ny;
    resamp_rrrf_execute_block(resamp, x, num_samples, y, &ny);

    // destroy resampler
    resamp_rrrf_destroy(resamp);

    //
    // create and run symbol synchronizer
    //

    // create symbol synchronizer
    symsync_rrrf sync = symsync_rrrf_create_rnyquist(ftype_rx, k, m, beta, num_filters);

    // set loop filter bandwidth
    symsync_rrrf_set_lf_bw(sync,bt);

    // execute on entire block of samples
    unsigned int nz;
    symsync_rrrf_execute(sync, y, ny, z, &nz);

    // destroy synchronizer
    symsync_rrrf_destroy(sync);

    // compute total delay through system
    // (initial filter + resampler + matched filter)
    unsigned int delay = m + 10 + m;

    if (liquid_autotest_verbose) {
        LOGI("symsync_rrrf_test(),\n");
        LOGI("    k       :   %u\n",      k);
        LOGI("    m       :   %u\n",      m);
        LOGI("    beta    :   %-8.4f\n",   beta);
        LOGI("    tau     :   %-8.4f\n",   tau);
        LOGI("    rate    :   %-12.8f\n",  rate);
        LOGI("output symbols:\n");
    }

    // compare (and print) results
    for (i=nz-num_symbols_test; i<nz; i++) {
        // compute error
        float err = fabsf( z[i] - s[i-delay] );

        // assert that error is below tolerance
        CONTEND_LESS_THAN( err, tol );

        // print formatted results if desired
        if (liquid_autotest_verbose) {
            LOGI("  sym_out(%4u) = %8.4f; %% {%8.4f} e = %12.8f %s\n",
                    i+1,
                    z[i],
                    s[i-delay],
                    err, err < tol ? "" : "*");
        }
    }

}

// autotest scenarios
void autotest_symsync_rrrf_scenario_0() { symsync_rrrf_test(2, 7, 0.35,  0.00, 1.0f    ); }
void autotest_symsync_rrrf_scenario_1() { symsync_rrrf_test(2, 7, 0.35, -0.25, 1.0f    ); }
void autotest_symsync_rrrf_scenario_2() { symsync_rrrf_test(2, 7, 0.35, -0.25, 1.0001f ); }
void autotest_symsync_rrrf_scenario_3() { symsync_rrrf_test(2, 7, 0.35, -0.25, 0.9999f ); }

