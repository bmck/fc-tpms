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
#include <string.h>
#include <math.h>
#include <sys/resource.h>

#include "liquid.internal.h"

#define PACKETIZER_DECODE_BENCH_API(N, CRC, FEC0, FEC1) \
(   struct rusage *_start,                              \
    struct rusage *_finish,                             \
    unsigned long int *_num_iterations)                 \
{ packetizer_decode_bench(_start, _finish, _num_iterations, N, CRC, FEC0, FEC1); }

// Helper function to keep code base small
void packetizer_decode_bench(struct rusage *     _start,
                             struct rusage *     _finish,
                             unsigned long int * _num_iterations,
                             unsigned int        _n,
                             crc_scheme          _crc,
                             fec_scheme          _fec0,
                             fec_scheme          _fec1)
{
    //
    unsigned int msg_dec_len = _n;
    unsigned int msg_enc_len = packetizer_compute_enc_msg_len(_n,_crc,_fec0,_fec1);

    // adjust number of iterations
    //  k-cycles/trial ~ 221 + 1.6125*msg_dec_len;
    // TODO: adjust iterations based on encoder types
    *_num_iterations *= 1000;
    *_num_iterations /= 221 + 1.6125*msg_dec_len;

    unsigned char msg_rec[msg_enc_len];
    unsigned char msg_dec[msg_dec_len];

    // initialize data
    unsigned long int i;
    for (i=0; i<msg_enc_len; i++) msg_rec[i] = rand() & 0xff;
    for (i=0; i<msg_dec_len; i++) msg_dec[i] = 0x00;

    // create packet generator
    packetizer q = packetizer_create(msg_dec_len, _crc, _fec0, _fec1);
    int crc_pass = 0;

    // start trials
    getrusage(RUSAGE_SELF, _start);
    for (i=0; i<(*_num_iterations); i++) {
        // decode packet
        crc_pass |= packetizer_decode(q, msg_rec, msg_dec);
        crc_pass |= packetizer_decode(q, msg_rec, msg_dec);
        crc_pass |= packetizer_decode(q, msg_rec, msg_dec);
        crc_pass |= packetizer_decode(q, msg_rec, msg_dec);
    }
    getrusage(RUSAGE_SELF, _finish);
    *_num_iterations *= 4;

    // clean up allocated objects
    packetizer_destroy(q);
}


//
// BENCHMARKS
//
void benchmark_packetizer_n16   PACKETIZER_DECODE_BENCH_API(16,   LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE)
void benchmark_packetizer_n32   PACKETIZER_DECODE_BENCH_API(32,   LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE)
void benchmark_packetizer_n64   PACKETIZER_DECODE_BENCH_API(64,   LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE)
void benchmark_packetizer_n128  PACKETIZER_DECODE_BENCH_API(128,  LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE)
void benchmark_packetizer_n256  PACKETIZER_DECODE_BENCH_API(256,  LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE)
void benchmark_packetizer_n512  PACKETIZER_DECODE_BENCH_API(512,  LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE)
void benchmark_packetizer_n1024 PACKETIZER_DECODE_BENCH_API(1024, LIQUID_CRC_NONE, LIQUID_FEC_NONE, LIQUID_FEC_NONE)

