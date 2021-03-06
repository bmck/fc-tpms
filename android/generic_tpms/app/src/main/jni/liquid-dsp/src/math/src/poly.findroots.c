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
// polynomial methods
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include "liquid.internal.h"

// debug polynomial root-finding methods?
#define LIQUID_POLY_FINDROOTS_DEBUG     0

//
// forward declaration of internal methods
//

// iterate over Bairstow's method, finding quadratic factor x^2 + u*x + v
void POLY(_findroots_bairstow_recursion)(T *          _p,
                                         unsigned int _k,
                                         T *          _p1,
                                         T *          _u,
                                         T *          _v);


// finds the complex roots of the polynomial
//  _p      :   polynomial array, ascending powers [size: _k x 1]
//  _k      :   polynomials length (poly order = _k - 1)
//  _roots  :   resulting complex roots [size: _k-1 x 1]
void POLY(_findroots)(T *          _p,
                      unsigned int _k,
                      TC *         _roots)
{
    // find roots of polynomial using Bairstow's method (more
    // accurate and reliable than Durand-Kerner)
    POLY(_findroots_bairstow)(_p,_k,_roots);
}

// finds the complex roots of the polynomial using the Durand-Kerner method
//  _p      :   polynomial array, ascending powers [size: _k x 1]
//  _k      :   polynomials length (poly order = _k - 1)
//  _roots  :   resulting complex roots [size: _k-1 x 1]
void POLY(_findroots_durandkerner)(T *          _p,
                                   unsigned int _k,
                                   TC *         _roots)
{
    if (_k < 2) {
        LOGE("%s_findroots_durandkerner(), order must be greater than 0\n", POLY_NAME);
        exit(1);
    } else if (_p[_k-1] != 1) {
        LOGE("%s_findroots_durandkerner(), _p[_k-1] must be equal to 1\n", POLY_NAME);
        exit(1);
    }

    unsigned int i;
    unsigned int num_roots = _k-1;
    T r0[num_roots];
    T r1[num_roots];

    // find intial magnitude
    float g     = 0.0f;
    float gmax  = 0.0f;
    for (i=0; i<_k; i++) {
        g = T_ABS(_p[i]);
        if (i==0 || g > gmax)
            gmax = g;
    }

    // initialize roots
    T t0 = 0.9f * (1 + gmax) * cexpf(_Complex_I*1.1526f);
    T t  = 1.0f;
    for (i=0; i<num_roots; i++) {
        r0[i] = t;
        t *= t0;
    }

    unsigned int max_num_iterations = 50;
    int continue_iterating = 1;
    unsigned int j, k;
    T f;
    T fp;
    //for (i=0; i<num_iterations; i++) {
    i = 0;
    while (continue_iterating) {
#if LIQUID_POLY_FINDROOTS_DEBUG
        LOGI("%s_findroots(), i=%3u :\n", POLY_NAME, i);
        for (j=0; j<num_roots; j++)
            LOGI("  r[%3u] = %12.8f + j*%12.8f\n", j, crealf(r0[j]), cimagf(r0[j]));
#endif
        for (j=0; j<num_roots; j++) {
            f = POLY(_val)(_p,_k,r0[j]);
            fp = 1;
            for (k=0; k<num_roots; k++) {
                if (k==j) continue;
                fp *= r0[j] - r0[k];
            }
            r1[j] = r0[j] - f / fp;
        }

        // stop iterating if roots have settled
        float delta=0.0f;
        T e;
        for (j=0; j<num_roots; j++) {
            e = r0[j] - r1[j];
            delta += crealf(e*conjf(e));
        }
        delta /= num_roots * gmax;
#if LIQUID_POLY_FINDROOTS_DEBUG
        LOGI("delta[%3u] = %12.4e\n", i, delta);
#endif

        if (delta < 1e-6f || i == max_num_iterations)
            continue_iterating = 0;

        memmove(r0, r1, num_roots*sizeof(T));
        i++;
    }

    for (i=0; i<_k; i++)
        _roots[i] = r1[i];
}

// finds the complex roots of the polynomial using Bairstow's method
//  _p      :   polynomial array, ascending powers [size: _k x 1]
//  _k      :   polynomials length (poly order = _k - 1)
//  _roots  :   resulting complex roots [size: _k-1 x 1]
void POLY(_findroots_bairstow)(T *          _p,
                               unsigned int _k,
                               TC *         _roots)
{
    T p0[_k];       // buffer 0
    T p1[_k];       // buffer 1
    T * p   = NULL; // input polynomial
    T * pr  = NULL; // output (reduced) polynomial

    unsigned int i;
    unsigned int k=0;   // output roots counter
    memmove(p0, _p, _k*sizeof(T));

    T u, v;

    unsigned int n = _k;        // input counter (decrementer)
    unsigned int r = _k % 2;    // polynomial length odd? (order even?)
    unsigned int L = (_k-r)/2;  // semi-length
    for (i=0; i<L-1+r; i++) {
        // set polynomial and reduced polynomial buffer pointers
        p  = (i % 2) == 0 ? p0 : p1;
        pr = (i % 2) == 0 ? p1 : p0;

        // initial estimates for u, v
        // TODO : ensure no division by zero
        if (p[n-1] == 0) {
            LOGE("warning: poly_findroots_bairstow(), irreducible polynomial");
            p[n-1] = 1e-12;
        }
        u = p[n-2] / p[n-1];
        v = p[n-3] / p[n-1];

        // compute factor using Bairstow's recursion
        POLY(_findroots_bairstow_recursion)(p,n,pr,&u,&v);

        // compute complex roots of x^2 + u*x + v
        TC r0 = 0.5f*(-u + csqrtf(u*u - 4.0*v));
        TC r1 = 0.5f*(-u - csqrtf(u*u - 4.0*v));

        // append result to output
        _roots[k++] = r0;
        _roots[k++] = r1;

#if LIQUID_POLY_FINDROOTS_DEBUG
        // print debugging info
        unsigned int j;
        LOGI("initial polynomial:\n");
        for (j=0; j<n; j++)
            LOGI("  p[%3u]  = %12.8f + j*%12.8f\n", j, crealf(p[j]), cimagf(p[j]));

        LOGI("polynomial factor: x^2 + u*x + v\n");
        LOGI("  u : %12.8f + j*%12.8f\n", crealf(u), cimagf(u));
        LOGI("  v : %12.8f + j*%12.8f\n", crealf(v), cimagf(v));

        LOGI("roots:\n");
        LOGI("  r0 : %12.8f + j*%12.8f\n", crealf(r0), cimagf(r0));
        LOGI("  r1 : %12.8f + j*%12.8f\n", crealf(r1), cimagf(r1));

        LOGI("reduced polynomial:\n");
        for (j=0; j<n-2; j++)
            LOGI("  pr[%3u] = %12.8f + j*%12.8f\n", j, crealf(pr[j]), cimagf(pr[j]));
#endif

        // decrement new (reduced) polynomial size by 2
        n -= 2;
    }

    if (r==0) {
#if LIQUID_POLY_FINDROOTS_DEBUG
        assert(n==2);
#endif
        _roots[k++] = -pr[0]/pr[1];
    }
}

// iterate over Bairstow's method, finding quadratic factor x^2 + u*x + v
//  _p      :   polynomial array, ascending powers [size: _k x 1]
//  _k      :   polynomials length (poly order = _k - 1)
//  _p1     :   reduced polynomial (output) [size: _k-2 x 1]
//  _u      :   input: initial estimate for u; output: resulting u
//  _v      :   input: initial estimate for v; output: resulting v
void POLY(_findroots_bairstow_recursion)(T *          _p,
                                         unsigned int _k,
                                         T *          _p1,
                                         T *          _u,
                                         T *          _v)
{
    // validate length
    if (_k < 3) {
        LOGE("findroots_bairstow_recursion(), invalid polynomial length: %u\n", _k);
        exit(1);
    }

    // initial estimates for u, v
    T u = *_u;
    T v = *_v;

    unsigned int n = _k-1;
    T c,d,g,h;
    T q;
    T du, dv;

    // reduced polynomials
    T b[_k];
    T f[_k];
    b[n] = b[n-1] = 0;
    f[n] = f[n-1] = 0;

    int i;
    unsigned int k=0;
    unsigned int max_num_iterations=50;
    int continue_iterating = 1;

    while (continue_iterating) {
        // update reduced polynomial coefficients
        for (i=n-2; i>=0; i--) {
            b[i] = _p[i+2] - u*b[i+1] - v*b[i+2];
            f[i] =  b[i+2] - u*f[i+1] - v*f[i+2];
        }
        c = _p[1] - u*b[0] - v*b[1];
        g =  b[1] - u*f[0] - v*f[1];
        d = _p[0] - v*b[0];
        h =  b[0] - v*f[0];

        // compute scaling factor
        q  = 1/(v*g*g + h*(h-u*g));

        // compute u, v steps
        du = - q*(-h*c   + g*d);
        dv = - q*(-g*v*c + (g*u-h)*d);

#if LIQUID_POLY_FINDROOTS_DEBUG
        // print debugging info
        LOGI("bairstow [%u] :\n", k);
        LOGI("  u     : %12.4e + j*%12.4e\n", crealf(u), cimagf(u));
        LOGI("  v     : %12.4e + j*%12.4e\n", crealf(v), cimagf(v));
        LOGI("  b     : \n");
        for (i=0; i<n-2; i++)
            LOGI("      %12.4e + j*%12.4e\n", crealf(b[i]), cimagf(b[i]));
        LOGI("  fb    : \n");
        for (i=0; i<n-2; i++)
            LOGI("      %12.4e + j*%12.4e\n", crealf(f[i]), cimagf(f[i]));
        LOGI("  c     : %12.4e + j*%12.4e\n", crealf(c), cimagf(c));
        LOGI("  g     : %12.4e + j*%12.4e\n", crealf(g), cimagf(g));
        LOGI("  d     : %12.4e + j*%12.4e\n", crealf(d), cimagf(d));
        LOGI("  h     : %12.4e + j*%12.4e\n", crealf(h), cimagf(h));
        LOGI("  q     : %12.4e + j*%12.4e\n", crealf(q), cimagf(q));
        LOGI("  du    : %12.4e + j*%12.4e\n", crealf(du), cimagf(du));
        LOGI("  dv    : %12.4e + j*%12.4e\n", crealf(dv), cimagf(dv));

        LOGI("  step : %12.4e + j*%12.4e\n", crealf(du+dv), cimagf(du+dv));
#endif

        // adjust u, v
        if (isnan(du) || isnan(dv)) {
            u *= 0.5f;
            v *= 0.5f;
        } else {
            u += du;
            v += dv;
        }

        // increment iteration counter
        k++;

        // exit conditions
        if (T_ABS(du+dv) < 1e-6f || k == max_num_iterations)
            continue_iterating = 0;
    }

    // set resulting reduced polynomial
    for (i=0; i<_k-2; i++)
        _p1[i] = b[i];

    // set output pairs
    *_u = u;
    *_v = v;

}

