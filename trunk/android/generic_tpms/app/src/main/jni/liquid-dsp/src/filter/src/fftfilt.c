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
// fftfilt : finite impulse response (FIR) filter using fast Fourier
//           transforms (FFTs)
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// defined:
//  FFTFILT()       name-mangling macro
//  T               coefficients type
//  WINDOW()        window macro
//  DOTPROD()       dotprod macro
//  PRINTVAL()      print macro

// fftfilt object structure
struct FFTFILT(_s) {
    TC * h;             // filter coefficients array [size; h_len x 1]
    unsigned int h_len; // filter length
    unsigned int n;     // input/output block size

    // internal memory arrays
    // TODO: make TI/TO type, but ensuring complex
    // TODO: use special format for fftfilt_rrrf type
    float complex * time_buf;   // time buffer [size: 2*n x 1]
    float complex * freq_buf;   // freq buffer [size: 2*n x 1]
    float complex * H;          // FFT of filter coefficients [size: 2*n x 1]
    float complex * w;          // overlap array [size: n x 1]

    // FFT objects
#ifdef LIQUID_FFTOVERRIDE
    fftplan fft;        // FFT object (forward)
    fftplan ifft;       // FFT object (inverse)
#else
    FFT_PLAN fft;       // FFT object (forward)
    FFT_PLAN ifft;      // FFT object (inverse)
#endif

    TC scale;           // output scaling factor
};

// create FFT-based FIR filter using external coefficients
//  _h      : filter coefficients [size: _h_len x 1]
//  _h_len  : filter length, _h_len > 0
//  _n      : block size = nfft/2, at least _h_len-1
FFTFILT() FFTFILT(_create)(TC *         _h,
                           unsigned int _h_len,
                           unsigned int _n)
{
    // validate input
    if (_h_len == 0) {
        LOGE("error: fftfilt_%s_create(), filter length must be greater than zero\n",
                EXTENSION_FULL);
        exit(1);
    } else if (_n < _h_len-1) {
        LOGE("error: fftfilt_%s_create(), block length must be greater than _h_len-1 (%u)\n",
                EXTENSION_FULL,
                _h_len-1);
        exit(1);
    }

    // create filter object and initialize
    FFTFILT() q = (FFTFILT()) malloc(sizeof(struct FFTFILT(_s)));
    q->h_len    = _h_len;
    q->n        = _n;

    // copy filter coefficients
    q->h = (TC *) malloc((q->h_len)*sizeof(TC));
    memmove(q->h, _h, _h_len*sizeof(TC));

    // allocate internal memory arrays
    q->time_buf = (float complex *) malloc((2*q->n)* sizeof(float complex)); // time buffer
    q->freq_buf = (float complex *) malloc((2*q->n)* sizeof(float complex)); // frequency buffer
    q->H        = (float complex *) malloc((2*q->n)* sizeof(float complex)); // FFT{ h }
    q->w        = (float complex *) malloc((  q->n)* sizeof(float complex)); // delay buffer

    // create internal FFT objects
#ifdef LIQUID_FFTOVERRIDE
    q->fft  = fft_create_plan(2*q->n, q->time_buf, q->freq_buf, LIQUID_FFT_FORWARD,  0);
    q->ifft = fft_create_plan(2*q->n, q->freq_buf, q->time_buf, LIQUID_FFT_BACKWARD, 0);
#else
    q->fft  = FFT_CREATE_PLAN(2*q->n, q->time_buf, q->freq_buf, FFT_DIR_FORWARD,  FFT_METHOD);
    q->ifft = FFT_CREATE_PLAN(2*q->n, q->freq_buf, q->time_buf, FFT_DIR_BACKWARD, FFT_METHOD);
#endif

    // compute FFT of filter coefficients and copy to internal H array
    unsigned int i;
    for (i=0; i<2*q->n; i++)
        q->time_buf[i] = (i < q->h_len) ? q->h[i] : 0;
    // time_buf > {FFT} > freq_buf
#ifdef LIQUID_FFTOVERRIDE
    fft_execute(q->fft);
#else
    FFT_EXECUTE(q->fft);
#endif
    memmove(q->H, q->freq_buf, 2*q->n*sizeof(float complex));

    // set default scaling
    FFTFILT(_set_scale)(q, 1);

    // reset filter state (clear buffer)
    FFTFILT(_reset)(q);

    // return object
    return q;
}

// destroy object, freeing all internally-allocated memory
void FFTFILT(_destroy)(FFTFILT() _q)
{
    // free internal arrays
    free(_q->h);                // filter coefficients
    free(_q->time_buf);         // buffer (time domain)
    free(_q->freq_buf);         // buffer (frequency domain)
    free(_q->H);                // frequency response of filter coefficients
    free(_q->w);                // output window buffer

    // destroy FFT objects
#ifdef LIQUID_FFTOVERRIDE
    fft_destroy_plan(_q->fft);       // forward transform
    fft_destroy_plan(_q->ifft);      // reverse transform
#else
    FFT_DESTROY_PLAN(_q->fft);  // forward transform
    FFT_DESTROY_PLAN(_q->ifft); // reverse transform
#endif

    // free main object
    free(_q);
}

// reset internal state of filter object
void FFTFILT(_reset)(FFTFILT() _q)
{
    // reset overlap window
    unsigned int i;
    for (i=0; i<_q->n; i++)
        _q->w[i] = 0;
}

// print filter object internals (taps, buffer)
void FFTFILT(_print)(FFTFILT() _q)
{
    LOGI("fftfilt_%s: [h_len=%u, n=%u]\n", EXTENSION_FULL, _q->h_len, _q->n);
    unsigned int i;
    unsigned int n = _q->h_len;
    for (i=0; i<n; i++) {
        LOGI("  h(%3u) = ", i+1);
        PRINTVAL_TC(_q->h[n-i-1],%12.8f);
        LOGI("\n");
    }

    // print scaling
    LOGI("  scale = ");
    PRINTVAL_TC(_q->scale,%12.8f);
    LOGI("\n");
}

// set output scaling for filter
void FFTFILT(_set_scale)(FFTFILT() _q,
                         TC        _scale)
{
    // set scale, normalized by fft size
    _q->scale = _scale / (float)(2*_q->n);
}

// execute the filter on internal buffer and coefficients
//  _q      : filter object
//  _x      : pointer to input data array  [size: _n x 1]
//  _y      : pointer to output data array [size: _n x 1]
void FFTFILT(_execute)(FFTFILT() _q,
                       TI *      _x,
                       TO *      _y)
{
    unsigned int i;

    // copy input
#if 0 //TI_COMPLEX
    memmove(_q->time_buf, _x, _q->n*sizeof(TI));
#else
    // manual copy for type conversion
    // TODO: use DCT or equivalent
    for (i=0; i<_q->n; i++)
        _q->time_buf[i] = _x[i];
#endif

    // pad end of time-domain buffer with zeros
    // TODO: not necessary to do this every time
#if 0
    memset(&_q->time_buf[_q->n], 0, _q->n*sizeof(TI));
#else
    for (i=0; i<_q->n; i++)
        _q->time_buf[_q->n + i] = 0;
#endif

    // run forward transform
#ifdef LIQUID_FFTOVERRIDE
    fft_execute(_q->fft);
#else
    FFT_EXECUTE(_q->fft);
#endif

    // compute inner product between FFT{ _x } and FFT{ H }
#if 1
    for (i=0; i<2*_q->n; i++)
        _q->freq_buf[i] *= _q->H[i];
#else
    // use SIMD vector extensions
# if TI_COMPLEX
    // complex floating-point inner product
    liquid_vectorcf_mul(_q->freq_buf, _q->H, 2*_q->n, _q->freq_buf);
# else
    // real floating-point inner product
    liquid_vectorf_mul(_q->freq_buf, _q->H, 2*_q->n, _q->freq_buf);
# endif
#endif

    // compute inverse transform
#ifdef LIQUID_FFTOVERRIDE
    fft_execute(_q->ifft);
#else
    FFT_EXECUTE(_q->ifft);
#endif

    // copy output summed with buffer and scaled
#if TI_COMPLEX
    for (i=0; i<_q->n; i++)
        _y[i] = (_q->time_buf[i] + _q->w[i]) * _q->scale;
#else
    // manual copy for type conversion
    // TODO: use DCT or equivalent
    for (i=0; i<_q->n; i++)
        _y[i] = (T) crealf(_q->time_buf[i] + _q->w[i]) * _q->scale;
#endif

    // copy buffer
    memmove(_q->w, &_q->time_buf[_q->n], _q->n*sizeof(float complex));
}

// return length of filter object's internal coefficients
unsigned int FFTFILT(_get_length)(FFTFILT() _q)
{
    return _q->h_len;
}

