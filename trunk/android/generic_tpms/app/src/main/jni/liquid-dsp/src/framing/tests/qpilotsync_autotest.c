//
// qpilotsync_example.c
//
// This example demonstrates...
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "autotest/autotest.h"
#include "liquid.h"

#define DEBUG_QPILOTSYNC_AUTOTEST 0

//
// AUTOTEST : test simple recovery of frame in noise
//
//  _ms             :   modulation scheme (e.g. LIQUID_MODEM_QPSK)
//  _payload_len    :   payload length [symbols]
//  _pilot_spacing  :   spacing between pilot symbols
//  _dphi           :   carrier frequency offset
//  _phi            :   carrier phase offset
//  _gamma          :   channel gain
//  _SNRdB          :   signal-to-noise ratio [dB]
void qpilotsync_test(modulation_scheme _ms,
                     unsigned int      _payload_len,
                     unsigned int      _pilot_spacing,
                     float             _dphi,
                     float             _phi,
                     float             _gamma,
                     float             _SNRdB)
{
    unsigned int i;
    // derived values
    float nstd = powf(10.0f, -_SNRdB/20.0f);

    // create pilot generator and synchronizer objects
    qpilotgen  pg = qpilotgen_create( _payload_len, _pilot_spacing);
    qpilotsync ps = qpilotsync_create(_payload_len, _pilot_spacing);

    // get frame length
    unsigned int frame_len = qpilotgen_get_frame_len(pg);

    // allocate arrays
    unsigned char payload_sym_tx[_payload_len]; // transmitted payload symbols
    float complex payload_tx    [_payload_len]; // transmitted payload samples
    float complex frame_tx      [frame_len];    // transmitted frame samples
    float complex frame_rx      [frame_len];    // received frame samples
    float complex payload_rx    [_payload_len]; // received payload samples
    unsigned char payload_sym_rx[_payload_len]; // received payload symbols

    // create modem objects for payload
    modem mod = modem_create(_ms);
    modem dem = modem_create(_ms);

    // assemble payload symbols
    for (i=0; i<_payload_len; i++) {
        // generate random symbol
        payload_sym_tx[i] = modem_gen_rand_sym(mod);

        // modulate
        modem_modulate(mod, payload_sym_tx[i], &payload_tx[i]);
    }

    // assemble frame
    qpilotgen_execute(pg, payload_tx, frame_tx);

    // add channel impairments
    for (i=0; i<frame_len; i++) {
        // add carrier offset
        frame_rx[i]  = frame_tx[i] * cexpf(_Complex_I*_dphi*i + _Complex_I*_phi);

        // add noise
        frame_rx[i] += nstd*(randnf() + _Complex_I*randnf())*M_SQRT1_2;

        // apply channel gain
        frame_rx[i] *= _gamma;
    }

    // recieve frame
    qpilotsync_execute(ps, frame_rx, payload_rx);

    // demodulate
    for (i=0; i<_payload_len; i++) {
        unsigned int sym_demod;
        modem_demodulate(dem, payload_rx[i], &sym_demod);
        payload_sym_rx[i] = (unsigned char)sym_demod;
    }

    // count errors
    unsigned int bit_errors = 0;
    for (i=0; i<_payload_len; i++)
        bit_errors += count_bit_errors(payload_sym_rx[i], payload_sym_tx[i]);

    // get estimates
    float dphi_hat  = qpilotsync_get_dphi(ps);
    float phi_hat   = qpilotsync_get_phi (ps);
    float gamma_hat = qpilotsync_get_gain(ps);

    if (liquid_autotest_verbose) {
        qpilotgen_print(pg);
        LOGI("  received bit errors : %u / %u\n", bit_errors, _payload_len * modem_get_bps(mod));
        LOGI("  dphi (carrier freq.): %12.8ff (expected %12.8f, error=%12.8f)\n", dphi_hat, _dphi, _dphi-dphi_hat);
        LOGI("  phi  (carrier phase): %12.8ff (expected %12.8f, error=%12.8f)\n",  phi_hat,  _phi, _phi-phi_hat);
        LOGI("  gamma (channel gain): %12.8ff (expected %12.8f, error=%12.8f)\n", gamma_hat, _gamma, _gamma-gamma_hat);
    }

    // check to see that frame was recovered
    CONTEND_DELTA   (   dphi_hat,  _dphi, 0.010f );
    CONTEND_DELTA   (    phi_hat,   _phi, 0.087f );  // 0.087 radians is about 5 degrees
    CONTEND_DELTA   (  gamma_hat, _gamma, 0.010f );
    CONTEND_EQUALITY( bit_errors, 0 );

    // destroy allocated objects
    qpilotgen_destroy(pg);
    qpilotsync_destroy(ps);
    modem_destroy(mod);
    modem_destroy(dem);

#if DEBUG_QPILOTSYNC_AUTOTEST
    // write symbols to output file for plotting
    char filename[256];
    sprintf(filename,"qpilotsync_autotest_%u_%u_debug.m", _payload_len, _pilot_spacing);
    FILE * fid = fopen(filename,"w");
    if (!fid) {
        LOGE("error: could not open '%s' for writing\n", filename);
        return;
    }
    LOGI("%% %s : auto-generated file\n", filename);
    LOGI("clear all;\n");
    LOGI("close all;\n");
    LOGI("payload_len = %u;\n", _payload_len);
    LOGI("frame_len   = %u;\n", frame_len);
    LOGI("frame_tx   = zeros(1,frame_len);\n");
    LOGI("payload_rx = zeros(1,payload_len);\n");
    for (i=0; i<frame_len; i++)
        LOGI("frame_rx(%6u) = %12.4e + 1i*%12.4e;\n", i+1, crealf(frame_rx[i]), cimagf(frame_rx[i]));
    for (i=0; i<_payload_len; i++)
        LOGI("payload_rx(%6u) = %12.4e + 1i*%12.4e;\n", i+1, crealf(payload_rx[i]), cimagf(payload_rx[i]));
    LOGI("figure('Color','white','position',[100 100 950 400]);\n");
    LOGI("subplot(1,2,1);\n");
    LOGI("  plot(real(frame_rx),  imag(frame_rx),  'x','MarkerSize',3);\n");
    LOGI("  axis([-1 1 -1 1]*1.5);\n");
    LOGI("  axis square;\n");
    LOGI("  grid on;\n");
    LOGI("  xlabel('real');\n");
    LOGI("  ylabel('imag');\n");
    LOGI("  title('received');\n");
    LOGI("subplot(1,2,2);\n");
    LOGI("  plot(real(payload_rx),imag(payload_rx),'x','MarkerSize',3);\n");
    LOGI("  axis([-1 1 -1 1]*1.5);\n");
    LOGI("  axis square;\n");
    LOGI("  grid on;\n");
    LOGI("  xlabel('real');\n");
    LOGI("  ylabel('imag');\n");
    LOGI("  title('recovered');\n");

    fclose(fid);
    LOGI("results written to '%s'\n", filename);
#endif
}

void autotest_qpilotsync_100_16() { qpilotsync_test(LIQUID_MODEM_QPSK, 100, 16, 0.07f, 1.2f, 0.7f, 40.0f); }
void autotest_qpilotsync_200_20() { qpilotsync_test(LIQUID_MODEM_QPSK, 200, 20, 0.07f, 1.2f, 0.7f, 40.0f); }
void autotest_qpilotsync_300_24() { qpilotsync_test(LIQUID_MODEM_QPSK, 300, 24, 0.07f, 1.2f, 0.7f, 40.0f); }
void autotest_qpilotsync_400_28() { qpilotsync_test(LIQUID_MODEM_QPSK, 400, 28, 0.07f, 1.2f, 0.7f, 40.0f); }
void autotest_qpilotsync_500_32() { qpilotsync_test(LIQUID_MODEM_QPSK, 500, 32, 0.07f, 1.2f, 0.7f, 40.0f); }

