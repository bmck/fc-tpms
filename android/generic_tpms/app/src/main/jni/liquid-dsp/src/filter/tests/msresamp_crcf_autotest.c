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

#include "autotest/autotest.h"
#include "liquid.h"

//
// AUTOTEST : test multi-stage arbitrary resampler
//
void autotest_msresamp_crcf()
{
    // options
    unsigned int m = 13;        // filter semi-length (filter delay)
    float r=0.127115323f;       // resampling rate (output/input)
    float As=60.0f;             // resampling filter stop-band attenuation [dB]
    unsigned int n=1200;        // number of input samples
    float fx=0.0254230646f;     // complex input sinusoid frequency (0.2*r)
    //float bw=0.45f;             // resampling filter bandwidth
    //unsigned int npfb=64;       // number of filters in bank (timing resolution)

    unsigned int i;

    // number of input samples (zero-padded)
    unsigned int nx = n + m;

    // output buffer with extra padding for good measure
    unsigned int y_len = (unsigned int) ceilf(1.1 * nx * r) + 4;

    // arrays
    float complex x[nx];
    float complex y[y_len];

    // create resampler
    msresamp_crcf q = msresamp_crcf_create(r,As);

    // generate input signal
    float wsum = 0.0f;
    for (i=0; i<nx; i++) {
        // compute window
        float w = i < n ? kaiser(i, n, 10.0f, 0.0f) : 0.0f;

        // apply window to complex sinusoid
        x[i] = cexpf(_Complex_I*2*M_PI*fx*i) * w;

        // accumulate window
        wsum += w;
    }

    // resample
    unsigned int ny=0;
    unsigned int nw;
    for (i=0; i<nx; i++) {
        // execute resampler, storing in output buffer
        msresamp_crcf_execute(q, &x[i], 1, &y[ny], &nw);

        // increment output size
        ny += nw;
    }

    // clean up allocated objects
    msresamp_crcf_destroy(q);

    //
    // analyze resulting signal
    //

    // check that the actual resampling rate is close to the target
    float r_actual = (float)ny / (float)nx;
    float fy = fx / r;      // expected output frequency

    // run FFT and ensure that carrier has moved and that image
    // frequencies and distortion have been adequately suppressed
    unsigned int nfft = 1 << liquid_nextpow2(ny);
    float complex yfft[nfft];   // fft input
    float complex Yfft[nfft];   // fft output
    for (i=0; i<nfft; i++)
        yfft[i] = i < ny ? y[i] : 0.0f;
    fft_run(nfft, yfft, Yfft, LIQUID_FFT_FORWARD, 0);
    fft_shift(Yfft, nfft);  // run FFT shift

    // find peak frequency
    float Ypeak = 0.0f;
    float fpeak = 0.0f;
    float max_sidelobe = -1e9f;     // maximum side-lobe [dB]
    float main_lobe_width = 0.07f;  // TODO: figure this out from Kaiser's equations
    for (i=0; i<nfft; i++) {
        // normalized output frequency
        float f = (float)i/(float)nfft - 0.5f;

        // scale FFT output appropriately
        Yfft[i] /= (r * wsum);
        float Ymag = 20*log10f( cabsf(Yfft[i]) );

        // find frequency location of maximum magnitude
        if (Ymag > Ypeak || i==0) {
            Ypeak = Ymag;
            fpeak = f;
        }

        // find peak side-lobe value, ignoring frequencies
        // within a certain range of signal frequency
        if ( fabsf(f-fy) > main_lobe_width )
            max_sidelobe = Ymag > max_sidelobe ? Ymag : max_sidelobe;
    }

    if (liquid_autotest_verbose) {
        // print results
        LOGI("  desired resampling rate   :   %12.8f\n", r);
        LOGI("  measured resampling rate  :   %12.8f    (%u/%u)\n", r_actual, ny, nx);
        LOGI("  peak spectrum             :   %12.8f dB (expected 0.0 dB)\n", Ypeak);
        LOGI("  peak frequency            :   %12.8f    (expected %-12.8f)\n", fpeak, fy);
        LOGI("  max sidelobe              :   %12.8f dB (expected at least %.2f dB)\n", max_sidelobe, -As);
    }
    CONTEND_DELTA(     r_actual, r,    0.01f ); // check actual output sample rate
    CONTEND_DELTA(     Ypeak,    0.0f, 0.25f ); // peak should be about 0 dB
    CONTEND_DELTA(     fpeak,    fy,   0.01f ); // peak frequency should be nearly 0.2
    CONTEND_LESS_THAN( max_sidelobe, -As );     // maximum side-lobe should be sufficiently low

#if 0
    // export results for debugging
    char filename[] = "msresamp_crcf_autotest.m";
    FILE*fid = fopen(filename,"w");
    LOGI("%% %s: auto-generated file\n",filename);
    LOGI("clear all;\n");
    LOGI("close all;\n");
    LOGI("r    = %12.8f;\n", r);
    LOGI("nx   = %u;\n", nx);
    LOGI("ny   = %u;\n", ny);
    LOGI("nfft = %u;\n", nfft);

    LOGI("Y = zeros(1,nfft);\n");
    for (i=0; i<nfft; i++)
        LOGI("Y(%3u) = %12.4e + j*%12.4e;\n", i+1, crealf(Yfft[i]), cimagf(Yfft[i]));

    LOGI("\n\n");
    LOGI("%% plot frequency-domain result\n");
    LOGI("f=[0:(nfft-1)]/nfft-0.5;\n");
    LOGI("figure;\n");
    LOGI("plot(f,20*log10(abs(Y)),'Color',[0.25 0.5 0.0],'LineWidth',2);\n");
    LOGI("grid on;\n");
    LOGI("xlabel('normalized frequency');\n");
    LOGI("ylabel('PSD [dB]');\n");
    LOGI("axis([-0.5 0.5 -120 20]);\n");

    fclose(fid);
    LOGI("results written to %s\n",filename);
#endif
}
