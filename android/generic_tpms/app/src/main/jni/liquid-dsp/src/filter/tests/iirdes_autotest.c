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
// References
//  [Ziemer:1998] Ziemer, Tranter, Fannin, "Signals & Systems,
//      Continuous and Discrete," 4th ed., Prentice Hall, Upper
//      Saddle River, NJ, 1998

#include "autotest/autotest.h"
#include "liquid.internal.h"

//
// AUTOTEST : design 2nd-order butterworth filter (design comes
//            from [Ziemer:1998] Example 9-7, pp. 440--442)
//
void autotest_iirdes_butter_2()
{
    // initialize variables
    unsigned int order = 2; // filter order
    float fc = 0.25f;       // normalized cutoff frequency
    float f0 = 0.0f;        // center frequency (ignored for low-pass filter)
    float Ap = 1.0f;        // pass-band ripple (ignored for Butterworth)
    float As = 40.0f;       // stop-band attenuation (ignored for Butterworth)
    float tol = 1e-6f;      // error tolerance

    // initialize pre-determined coefficient array
    // for 2^nd-order low-pass Butterworth filter
    // with cutoff frequency 0.25
    float a_test[3] = {
        1.0f,
        0.0f,
        0.171572875253810f};
    float b_test[3] = {
        0.292893218813452f,
        0.585786437626905f,
        0.292893218813452f};

    // output coefficients
    float a[3];
    float b[3];

    // design butterworth filter
    liquid_iirdes(LIQUID_IIRDES_BUTTER,
                  LIQUID_IIRDES_LOWPASS,
                  LIQUID_IIRDES_TF,
                  order,
                  fc, f0,
                  Ap, As,
                  b, a);

    // Ensure data are equal to within tolerance
    unsigned int i;
    for (i=0; i<3; i++) {
        CONTEND_DELTA( b[i], b_test[i], tol );
        CONTEND_DELTA( a[i], a_test[i], tol );
    }
}

//
// AUTOTEST : complex pair, n=6
//

void autotest_iirdes_cplxpair_n6()
{
    float tol = 1e-8f;

    //
    float complex r[6] = {
       0.980066577841242 + 0.198669330795061 * _Complex_I,
       5.000000000000000 + 0.000000000000000 * _Complex_I,
      -0.416146836547142 + 0.909297426825682 * _Complex_I,
       0.980066577841242 - 0.198669330795061 * _Complex_I,
       0.300000000000000 + 0.000000000000000 * _Complex_I,
      -0.416146836547142 - 0.909297426825682 * _Complex_I
    };

    float complex p[6];

    float complex ptest[6] = {
      -0.416146836547142 - 0.909297426825682 * _Complex_I,
      -0.416146836547142 + 0.909297426825682 * _Complex_I,
       0.980066577841242 - 0.198669330795061 * _Complex_I,
       0.980066577841242 + 0.198669330795061 * _Complex_I,
       0.300000000000000 + 0.000000000000000 * _Complex_I,
       5.000000000000000 + 0.000000000000000 * _Complex_I
    };

    // compute complex pairs
    liquid_cplxpair(r,6,1e-6f,p);

    unsigned int i;

    if (liquid_autotest_verbose) {
        LOGI("complex set:\n");
        for (i=0; i<6; i++)
            LOGI("  r[%3u] : %12.8f + j*%12.8f\n", i, crealf(r[i]), cimagf(r[i]));

        LOGI("complex pairs:\n");
        for (i=0; i<6; i++)
            LOGI("  p[%3u] : %12.8f + j*%12.8f\n", i, crealf(p[i]), cimagf(p[i]));
    }

    // run test
    for (i=0; i<6; i++) {
        CONTEND_DELTA( crealf(p[i]), crealf(ptest[i]), tol );
        CONTEND_DELTA( cimagf(p[i]), cimagf(ptest[i]), tol );
    }
}


//
// AUTOTEST : complex pair, n=20
//

void autotest_iirdes_cplxpair_n20()
{
    float tol = 1e-8f;

    //
    float complex r[20] = {
      -0.340396183901119 + 1.109902927794652 * _Complex_I,
       1.148964416793990 + 0.000000000000000 * _Complex_I,
       0.190037889511651 + 0.597517076404221 * _Complex_I,
      -0.340396183901119 - 1.109902927794652 * _Complex_I,
       0.890883293686046 + 0.000000000000000 * _Complex_I,
      -0.248338528396292 - 0.199390430636670 * _Complex_I,
       0.190037889511651 - 0.597517076404221 * _Complex_I,
       0.003180396218998 + 0.000000000000000 * _Complex_I,
       0.261949046540733 - 0.739400953405199 * _Complex_I,
       0.261949046540733 + 0.739400953405199 * _Complex_I,
       0.309342570837113 + 0.000000000000000 * _Complex_I,
       0.035516103001236 + 0.000000000000000 * _Complex_I,
      -0.184159864176452 - 0.240335024546875 * _Complex_I,
      -0.485244526317243 + 0.452251520655749 * _Complex_I,
      -0.485244526317243 - 0.452251520655749 * _Complex_I,
      -0.581633365450190 + 0.000000000000000 * _Complex_I,
      -0.248338528396292 + 0.199390430636670 * _Complex_I,
      -0.184159864176452 + 0.240335024546875 * _Complex_I,
       1.013685316242435 + 0.000000000000000 * _Complex_I,
      -0.089598596934739 + 0.000000000000000 * _Complex_I
    };

    float complex p[20];

    float complex ptest[20] = {
      -0.485244526317243 - 0.452251520655749 * _Complex_I,
      -0.485244526317243 + 0.452251520655749 * _Complex_I,
      -0.340396183901119 - 1.109902927794652 * _Complex_I,
      -0.340396183901119 + 1.109902927794652 * _Complex_I,
      -0.248338528396292 - 0.199390430636670 * _Complex_I,
      -0.248338528396292 + 0.199390430636670 * _Complex_I,
      -0.184159864176452 - 0.240335024546875 * _Complex_I,
      -0.184159864176452 + 0.240335024546875 * _Complex_I,
       0.190037889511651 - 0.597517076404221 * _Complex_I,
       0.190037889511651 + 0.597517076404221 * _Complex_I,
       0.261949046540733 - 0.739400953405199 * _Complex_I,
       0.261949046540733 + 0.739400953405199 * _Complex_I,
      -0.581633365450190 + 0.000000000000000 * _Complex_I,
      -0.089598596934739 + 0.000000000000000 * _Complex_I,
       0.003180396218998 + 0.000000000000000 * _Complex_I,
       0.035516103001236 + 0.000000000000000 * _Complex_I,
       0.309342570837113 + 0.000000000000000 * _Complex_I,
       0.890883293686046 + 0.000000000000000 * _Complex_I,
       1.013685316242435 + 0.000000000000000 * _Complex_I,
       1.148964416793990 + 0.000000000000000 * _Complex_I
    };

    // compute complex pairs
    liquid_cplxpair(r,20,1e-6f,p);

    unsigned int i;

    if (liquid_autotest_verbose) {
        LOGI("complex set:\n");
        for (i=0; i<20; i++)
            LOGI("  r[%3u] : %12.8f + j*%12.8f\n", i, crealf(r[i]), cimagf(r[i]));

        LOGI("complex pairs:\n");
        for (i=0; i<20; i++)
            LOGI("  p[%3u] : %12.8f + j*%12.8f\n", i, crealf(p[i]), cimagf(p[i]));
    }

    // run test
    for (i=0; i<20; i++) {
        CONTEND_DELTA( crealf(p[i]), crealf(ptest[i]), tol );
        CONTEND_DELTA( cimagf(p[i]), cimagf(ptest[i]), tol );
    }
}

//
// AUTOTEST :
//
void autotest_iirdes_dzpk2sosf()
{
    unsigned int n=4;
    float fc = 0.25f;

    unsigned int i;
    unsigned int L = n % 2 ? (n+1)/2 : n/2;
    float B[3*L];
    float A[3*L];

    float complex za[n];    // analog zeros
    float complex pa[n];    // analog poles
    float complex ka;       // analog gain
    butter_azpkf(n,za,pa,&ka);

    float complex zd[n];    // digital zeros
    float complex pd[n];    // digital poles
    float complex kd;       // digital gain
    float m = 1 / tanf(M_PI * fc);
    bilinear_zpkf(za,  0,
                  pa,  n,
                  ka,  m,
                  zd, pd, &kd);

    if (liquid_autotest_verbose) {
        LOGI("poles (digital):\n");
        for (i=0; i<n; i++)
            LOGI("  pd[%3u] = %12.8f + j*%12.8f\n", i, crealf(pd[i]), cimagf(pd[i]));

        LOGI("zeros (digital):\n");
        for (i=0; i<n; i++)
            LOGI("  zd[%3u] = %12.8f + j*%12.8f\n", i, crealf(zd[i]), cimagf(zd[i]));
    }

    iirdes_dzpk2sosf(zd,pd,n,kd,B,A);

    if (liquid_autotest_verbose) {
        LOGI("B:\n");
        for (i=0; i<L; i++)
            LOGI("  %12.8f %12.8f %12.8f\n", B[3*i+0], B[3*i+1], B[3*i+2]);

        LOGI("A:\n");
        for (i=0; i<L; i++)
            LOGI("  %12.8f %12.8f %12.8f\n", A[3*i+0], A[3*i+1], A[3*i+2]);
    }
}

//
// AUTOTEST : iirdes_isstable
//
void autotest_iirdes_isstable_n2_yes()
{
    // initialize pre-determined coefficient array
    // for 2^nd-order low-pass Butterworth filter
    // with cutoff frequency 0.25
    float a[3] = {
        1.0f,
        0.0f,
        0.171572875253810f};
    float b[3] = {
        0.292893218813452f,
        0.585786437626905f,
        0.292893218813452f};

    int stable = iirdes_isstable(b,a,3);
    CONTEND_EQUALITY( stable, 1 );
}


//
// AUTOTEST : iirdes_isstable
//
void autotest_iirdes_isstable_n2_no()
{
    // initialize unstable filter
    float a[3] = {
        1.0f,
        0.0f,
        1.171572875253810f};
    float b[3] = {
        0.292893218813452f,
        0.585786437626905f,
        0.292893218813452f};

    int stable = iirdes_isstable(b,a,3);
    CONTEND_EQUALITY( stable, 0 );
}

