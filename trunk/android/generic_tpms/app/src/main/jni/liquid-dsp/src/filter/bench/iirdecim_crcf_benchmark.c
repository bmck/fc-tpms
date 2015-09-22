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

#include <sys/resource.h>
#include "liquid.h"

// Helper function to keep code base small
void iirdecim_crcf_bench(struct rusage *     _start,
                         struct rusage *     _finish,
                         unsigned long int * _num_iterations,
                         unsigned int        _M,
                         unsigned int        _order)
{
    // normalize number of iterations
    *_num_iterations /= _order;
    if (*_num_iterations < 1) *_num_iterations = 1;

    // create decimator from prototype
    liquid_iirdes_filtertype ftype  = LIQUID_IIRDES_BUTTER;
    liquid_iirdes_bandtype   btype  = LIQUID_IIRDES_LOWPASS;
    liquid_iirdes_format     format = LIQUID_IIRDES_SOS;
    float fc =  0.5f / (float)_M;
    float f0 =  0.0f;
    float Ap =  0.1f;
    float As = 60.0f;
    iirdecim_crcf q = iirdecim_crcf_create_prototype(_M,ftype,btype,format,_order,fc,f0,Ap,As);

    // initialize input
    float complex x[_M];
    unsigned int i;
    for (i=0; i<_M; i++)
        x[i] = (i%2) ? 1.0f : -1.0f;

    float complex y;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        iirdecim_crcf_execute(q, x, &y);
        iirdecim_crcf_execute(q, x, &y);
        iirdecim_crcf_execute(q, x, &y);
        iirdecim_crcf_execute(q, x, &y);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    iirdecim_crcf_destroy(q);
}

#define IIRDECIM_CRCF_BENCHMARK_API(M,ORDER)    \
(   struct rusage *_start,                      \
    struct rusage *_finish,                     \
    unsigned long int *_num_iterations)         \
{ iirdecim_crcf_bench(_start, _finish, _num_iterations, M, ORDER); }

void benchmark_iirdecim_crcf_M2     IIRDECIM_CRCF_BENCHMARK_API(2, 5)
void benchmark_iirdecim_crcf_M4     IIRDECIM_CRCF_BENCHMARK_API(4, 5)
void benchmark_iirdecim_crcf_M8     IIRDECIM_CRCF_BENCHMARK_API(8, 5)
void benchmark_iirdecim_crcf_M16    IIRDECIM_CRCF_BENCHMARK_API(16,5)
void benchmark_iirdecim_cccf_M32    IIRDECIM_CRCF_BENCHMARK_API(32,5)

