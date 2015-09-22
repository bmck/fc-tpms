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

#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include "liquid.h"

#define MODEM_MODULATE_BENCH_API(MS)    \
(   struct rusage *_start,              \
    struct rusage *_finish,             \
    unsigned long int *_num_iterations) \
{ modem_modulate_bench(_start, _finish, _num_iterations, MS); }

// Helper function to keep code base small
void modem_modulate_bench(struct rusage *_start,
                          struct rusage *_finish,
                          unsigned long int *_num_iterations,
                          modulation_scheme _ms)
{
    // normalize number of iterations
    switch (_ms) {
    case LIQUID_MODEM_UNKNOWN:
        LOGE("error: modem_modulate_bench(), unknown modem scheme\n");
        exit(1);
    case LIQUID_MODEM_BPSK:     *_num_iterations *= 64;     break;
    case LIQUID_MODEM_QPSK:     *_num_iterations *= 64;     break;
    case LIQUID_MODEM_OOK:      *_num_iterations *= 64;     break;
    case LIQUID_MODEM_SQAM32:   *_num_iterations *= 16;     break;
    case LIQUID_MODEM_SQAM128:  *_num_iterations *= 16;     break;
    case LIQUID_MODEM_V29:      *_num_iterations *= 100;    break;
    case LIQUID_MODEM_ARB16OPT: *_num_iterations *= 100;    break;
    case LIQUID_MODEM_ARB32OPT: *_num_iterations *= 100;    break;
    case LIQUID_MODEM_ARB64OPT: *_num_iterations *= 100;    break;
    case LIQUID_MODEM_ARB128OPT: *_num_iterations *= 100;   break;
    case LIQUID_MODEM_ARB256OPT: *_num_iterations *= 100;   break;
    case LIQUID_MODEM_ARB64VT:  *_num_iterations *= 100;    break;
    default:;
        *_num_iterations *= 32;
    }

    if (*_num_iterations < 1) *_num_iterations = 1;


    // initialize modulator
    modem mod = modem_create(_ms);

    float complex x;
    unsigned int symbol_in = 0;

    unsigned long int i;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        modem_modulate(mod, symbol_in, &x);
        modem_modulate(mod, symbol_in, &x);
        modem_modulate(mod, symbol_in, &x);
        modem_modulate(mod, symbol_in, &x);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    modem_destroy(mod);
}

// specific modems
void benchmark_modulate_bpsk    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_BPSK)
void benchmark_modulate_qpsk    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QPSK)
void benchmark_modulate_ook     MODEM_MODULATE_BENCH_API(LIQUID_MODEM_OOK)
void benchmark_modulate_sqam32  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_SQAM32)
void benchmark_modulate_sqam128 MODEM_MODULATE_BENCH_API(LIQUID_MODEM_SQAM128)

// ASK
void benchmark_modulate_ask2    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ASK2)
void benchmark_modulate_ask4    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ASK4)
void benchmark_modulate_ask8    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ASK8)
void benchmark_modulate_ask16   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ASK16)

// PSK
void benchmark_modulate_psk2    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK2)
void benchmark_modulate_psk4    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK4)
void benchmark_modulate_psk8    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK8)
void benchmark_modulate_psk16   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK16)
void benchmark_modulate_psk32   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK32)
void benchmark_modulate_psk64   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_PSK64)

// Differential PSK
void benchmark_modulate_dpsk2   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK2)
void benchmark_modulate_dpsk4   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK4)
void benchmark_modulate_dpsk8   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK8)
void benchmark_modulate_dpsk16  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK16)
void benchmark_modulate_dpsk32  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK32)
void benchmark_modulate_dpsk64  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_DPSK64)

// QAM
void benchmark_modulate_qam4    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM4)
void benchmark_modulate_qam8    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM8)
void benchmark_modulate_qam16   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM16)
void benchmark_modulate_qam32   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM32)
void benchmark_modulate_qam64   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM64)
void benchmark_modulate_qam128  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM128)
void benchmark_modulate_qam256  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_QAM256)

// APSK
void benchmark_modulate_apsk4   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK4)
void benchmark_modulate_apsk8   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK8)
void benchmark_modulate_apsk16  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK16)
void benchmark_modulate_apsk32  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK32)
void benchmark_modulate_apsk64  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK64)
void benchmark_modulate_apsk128 MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK128)
void benchmark_modulate_apsk256 MODEM_MODULATE_BENCH_API(LIQUID_MODEM_APSK256)

// ARB
void benchmark_modulate_arbV29    MODEM_MODULATE_BENCH_API(LIQUID_MODEM_V29)
void benchmark_modulate_arb16opt  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB16OPT)
void benchmark_modulate_arb32opt  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB32OPT)
void benchmark_modulate_arb64opt  MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB64OPT)
void benchmark_modulate_arb128opt MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB128OPT)
void benchmark_modulate_arb256opt MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB256OPT)
void benchmark_modulate_arb64vt   MODEM_MODULATE_BENCH_API(LIQUID_MODEM_ARB64VT)

