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
// convolutional code (macros)
//

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "liquid.internal.h"

#define VERBOSE_FEC_CONV_PUNCTURED    0

#if LIBFEC_ENABLED
#include "fec.h"

fec fec_conv_punctured_create(fec_scheme _fs)
{
    fec q = (fec) malloc(sizeof(struct fec_s));

    q->scheme = _fs;
    q->rate = fec_get_rate(q->scheme);

    q->encode_func      = &fec_conv_punctured_encode;
    q->decode_func      = &fec_conv_punctured_decode_hard;
    q->decode_soft_func = &fec_conv_punctured_decode_soft;

    switch (q->scheme) {
    case LIQUID_FEC_CONV_V27P23:   fec_conv_init_v27p23(q);    break;
    case LIQUID_FEC_CONV_V27P34:   fec_conv_init_v27p34(q);    break;
    case LIQUID_FEC_CONV_V27P45:   fec_conv_init_v27p45(q);    break;
    case LIQUID_FEC_CONV_V27P56:   fec_conv_init_v27p56(q);    break;
    case LIQUID_FEC_CONV_V27P67:   fec_conv_init_v27p67(q);    break;
    case LIQUID_FEC_CONV_V27P78:   fec_conv_init_v27p78(q);    break;

    case LIQUID_FEC_CONV_V29P23:   fec_conv_init_v29p23(q);    break;
    case LIQUID_FEC_CONV_V29P34:   fec_conv_init_v29p34(q);    break;
    case LIQUID_FEC_CONV_V29P45:   fec_conv_init_v29p45(q);    break;
    case LIQUID_FEC_CONV_V29P56:   fec_conv_init_v29p56(q);    break;
    case LIQUID_FEC_CONV_V29P67:   fec_conv_init_v29p67(q);    break;
    case LIQUID_FEC_CONV_V29P78:   fec_conv_init_v29p78(q);    break;
    default:
        LOGE("error: fec_conv_punctured_create(), invalid type\n");
        exit(1);
    }

    // convolutional-specific decoding
    q->num_dec_bytes = 0;
    q->enc_bits = NULL;
    q->vp = NULL;

    return q;
}

void fec_conv_punctured_destroy(fec _q)
{
    // delete viterbi decoder
    if (_q->vp != NULL)
        _q->delete_viterbi(_q->vp);

    free(_q);
}

void fec_conv_punctured_encode(fec _q,
                               unsigned int _dec_msg_len,
                               unsigned char *_msg_dec,
                               unsigned char *_msg_enc)
{
    unsigned int i,j,r; // bookkeeping
    unsigned int sr=0;  // convolutional shift register
    unsigned int n=0;   // output bit counter
    unsigned int p=0;   // puncturing matrix column index

    unsigned char bit;
    unsigned char byte_in;
    unsigned char byte_out=0;

    for (i=0; i<_dec_msg_len; i++) {
        byte_in = _msg_dec[i];

        // break byte into individual bits
        for (j=0; j<8; j++) {
            // shift bit starting with most significant
            bit = (byte_in >> (7-j)) & 0x01;
            sr = (sr << 1) | bit;

            // compute parity bits for each polynomial
            for (r=0; r<_q->R; r++) {
                // enable output determined by puncturing matrix
                if (_q->puncturing_matrix[r*(_q->P)+p]) {
                    byte_out = (byte_out<<1) | parity(sr & _q->poly[r]);
                    _msg_enc[n/8] = byte_out;
                    n++;
                } else {
                }
            }

            // update puncturing matrix column index
            p = (p+1) % _q->P;
        }
    }
    //LOGI("\n");
    //LOGI("*** n = %u\n", n);

    // tail bits
    for (i=0; i<_q->K-1; i++) {
        // shift register: push zeros
        sr = (sr << 1);

        // compute parity bits for each polynomial
        for (r=0; r<_q->R; r++) {
            if (_q->puncturing_matrix[r*(_q->P)+p]) {
                byte_out = (byte_out<<1) | parity(sr & _q->poly[r]);
                _msg_enc[n/8] = byte_out;
                n++;
            }
        }

        // update puncturing matrix column index
        p = (p+1) % _q->P;
    }
    //LOGI("+++ n = %u\n", n);

    // ensure even number of bytes
    while (n%8) {
        // shift zeros
        byte_out <<= 1;
        _msg_enc[n/8] = byte_out;
        n++;
    }

    //LOGI("n = %u (expected %u)\n", n, 8*fec_get_enc_msg_length(LIQUID_FEC_CONV(_mode),_dec_msg_len));
    assert(n == 8*fec_get_enc_msg_length(_q->scheme,_dec_msg_len));
}

//unsigned int
void fec_conv_punctured_decode_hard(fec _q,
                                    unsigned int _dec_msg_len,
                                    unsigned char *_msg_enc,
                                    unsigned char *_msg_dec)
{
    // re-allocate resources if necessary
    fec_conv_punctured_setlength(_q, _dec_msg_len);

    // unpack bytes, adding erasures at punctured indices
    unsigned int num_dec_bits = _q->num_dec_bytes * 8 + _q->K - 1;
    unsigned int num_enc_bits = num_dec_bits * _q->R;
    unsigned int i,r;
    unsigned int n=0;   // input byte index
    unsigned int k=0;   // intput bit index (0<=k<8)
    unsigned int p=0;   // puncturing matrix column index
    unsigned char bit;
    unsigned char byte_in = _msg_enc[n];
    for (i=0; i<num_enc_bits; i+=_q->R) {
        //
        for (r=0; r<_q->R; r++) {
            if (_q->puncturing_matrix[r*(_q->P)+p]) {
                // push bit from input
                bit = (byte_in >> (7-k)) & 0x01;
                _q->enc_bits[i+r] = bit ? LIQUID_SOFTBIT_1 : LIQUID_SOFTBIT_0;
                k++;
                if (k==8) {
                    k = 0;
                    n++;
                    byte_in = _msg_enc[n];
                }
            } else {
                // push erasure
                _q->enc_bits[i+r] = LIQUID_SOFTBIT_ERASURE;
            }
        }
        p = (p+1) % _q->P;
    }

#if VERBOSE_FEC_CONV_PUNCTURED
    unsigned int ii;
    LOGI("msg encoded (bits):\n");
    for (ii=0; ii<num_enc_bits; ii++) {
        LOGI("%3u ", _q->enc_bits[ii]);
        if (((ii+1)%8)==0)
            LOGI("\n");
    }
    LOGI("\n");
#endif

    // run decoder
    _q->init_viterbi(_q->vp,0);
    // TODO : check to see if this shouldn't be num_enc_bits (punctured)
    _q->update_viterbi_blk(_q->vp, _q->enc_bits, 8*_q->num_dec_bytes+_q->K-1);
    _q->chainback_viterbi(_q->vp, _msg_dec, 8*_q->num_dec_bytes, 0);

#if VERBOSE_FEC_CONV_PUNCTURED
    for (ii=0; ii<_dec_msg_len; ii++)
        LOGI("%.2x ", _msg_dec[ii]);
    LOGI("\n");
#endif
}

//unsigned int
void fec_conv_punctured_decode_soft(fec _q,
                                    unsigned int _dec_msg_len,
                                    unsigned char *_msg_enc,
                                    unsigned char *_msg_dec)
{
    // re-allocate resources if necessary
    fec_conv_punctured_setlength(_q, _dec_msg_len);

    // unpack bytes, adding erasures at punctured indices
    unsigned int num_dec_bits = _q->num_dec_bytes * 8 + _q->K - 1;
    unsigned int num_enc_bits = num_dec_bits * _q->R;
    unsigned int i,r;
    unsigned int n=0;   // input soft bit index
    unsigned int p=0;   // puncturing matrix column index
    for (i=0; i<num_enc_bits; i+=_q->R) {
        //
        for (r=0; r<_q->R; r++) {
            if (_q->puncturing_matrix[r*(_q->P)+p]) {
                // push bit from input
                _q->enc_bits[i+r] = _msg_enc[n++];
            } else {
                // push erasure
                _q->enc_bits[i+r] = LIQUID_SOFTBIT_ERASURE;
            }
        }
        p = (p+1) % _q->P;
    }

#if VERBOSE_FEC_CONV_PUNCTURED
    unsigned int ii;
    LOGI("msg encoded (bits):\n");
    for (ii=0; ii<num_enc_bits; ii++) {
        LOGI("%3u ", _q->enc_bits[ii]);
        if (((ii+1)%8)==0)
            LOGI("\n");
    }
    LOGI("\n");
#endif

    // run decoder
    _q->init_viterbi(_q->vp,0);
    // TODO : check to see if this shouldn't be num_enc_bits (punctured)
    _q->update_viterbi_blk(_q->vp, _q->enc_bits, 8*_q->num_dec_bytes+_q->K-1);
    _q->chainback_viterbi(_q->vp, _msg_dec, 8*_q->num_dec_bytes, 0);

#if VERBOSE_FEC_CONV_PUNCTURED
    for (ii=0; ii<_dec_msg_len; ii++)
        LOGI("%.2x ", _msg_dec[ii]);
    LOGI("\n");
#endif
}

void fec_conv_punctured_setlength(fec _q, unsigned int _dec_msg_len)
{
    // re-allocate resources as necessary
    unsigned int num_dec_bytes = _dec_msg_len;

    // return if length has not changed
    if (num_dec_bytes == _q->num_dec_bytes)
        return;

    // reset number of framebits
    _q->num_dec_bytes = num_dec_bytes;
    _q->num_enc_bytes = fec_get_enc_msg_length(_q->scheme,
                                               _dec_msg_len);

    // puncturing: need to expand to full length (decoder
    //             injects erasures at punctured values)
    unsigned int num_dec_bits = 8*_q->num_dec_bytes;
    unsigned int n = num_dec_bits + _q->K - 1;
    unsigned int num_enc_bits = n*(_q->R);
#if VERBOSE_FEC_CONV_PUNCTURED
    LOGI("(re)creating viterbi decoder, %u frame bytes\n", num_dec_bytes);
    LOGI("  num decoded bytes         :   %u\n", _q->num_dec_bytes);
    LOGI("  num encoded bytes         :   %u\n", _q->num_enc_bytes);
    LOGI("  num decoded bits          :   %u\n", num_dec_bits);
    LOGI("  num decoded bits (padded) :   %u\n", n);
    LOGI("  num encoded bits (full)   :   %u\n", num_enc_bits);
#endif

    // delete old decoder if necessary
    if (_q->vp != NULL)
        _q->delete_viterbi(_q->vp);

    // re-create / re-allocate memory buffers
    _q->vp = _q->create_viterbi(8*_q->num_dec_bytes);
    _q->enc_bits = (unsigned char*) realloc(_q->enc_bits,
                                            num_enc_bits*sizeof(unsigned char));

}

//
// internal
//

void fec_conv_init_v27p23(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v27(_q);

    _q->P = 2;
    _q->puncturing_matrix = fec_conv27p23_matrix;
}

void fec_conv_init_v27p34(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v27(_q);

    _q->P = 3;
    _q->puncturing_matrix = fec_conv27p34_matrix;
}

void fec_conv_init_v27p45(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v27(_q);

    _q->P = 4;
    _q->puncturing_matrix = fec_conv27p45_matrix;
}

void fec_conv_init_v27p56(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v27(_q);

    _q->P = 5;
    _q->puncturing_matrix = fec_conv27p56_matrix;
}


void fec_conv_init_v27p67(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v27(_q);

    _q->P = 6;
    _q->puncturing_matrix = fec_conv27p67_matrix;
}

void fec_conv_init_v27p78(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v27(_q);

    _q->P = 7;
    _q->puncturing_matrix = fec_conv27p78_matrix;
}


void fec_conv_init_v29p23(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v29(_q);

    _q->P = 2;
    _q->puncturing_matrix = fec_conv29p23_matrix;
}

void fec_conv_init_v29p34(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v29(_q);

    _q->P = 3;
    _q->puncturing_matrix = fec_conv29p34_matrix;
}

void fec_conv_init_v29p45(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v29(_q);

    _q->P = 4;
    _q->puncturing_matrix = fec_conv29p45_matrix;
}

void fec_conv_init_v29p56(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v29(_q);

    _q->P = 5;
    _q->puncturing_matrix = fec_conv29p56_matrix;
}


void fec_conv_init_v29p67(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v29(_q);

    _q->P = 6;
    _q->puncturing_matrix = fec_conv29p67_matrix;
}

void fec_conv_init_v29p78(fec _q)
{
    // initialize R, K, polynomial, and viterbi methods
    fec_conv_init_v29(_q);

    _q->P = 7;
    _q->puncturing_matrix = fec_conv29p78_matrix;
}


#else   // LIBFEC_ENABLED

fec fec_conv_punctured_create(fec_scheme _fs)
{
    return NULL;
}

void fec_conv_punctured_destroy(fec _q)
{
}

void fec_conv_punctured_encode(fec _q,
                               unsigned int _dec_msg_len,
                               unsigned char *_msg_dec,
                               unsigned char *_msg_enc)
{
}

//unsigned int
void fec_conv_punctured_decode(fec _q,
                               unsigned int _dec_msg_len,
                               unsigned char *_msg_enc,
                               unsigned char *_msg_dec)
{
}

#endif  // LIBFEC_ENABLED

