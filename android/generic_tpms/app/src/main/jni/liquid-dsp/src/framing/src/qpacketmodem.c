/*
 * Copyright (c) 2007 - 2015 Joseph Gaeddert
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
// qpacketmodem.c
//
// convenient modulator/demodulator and packet encoder/decoder combination
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <complex.h>

#include "liquid.internal.h"

// enable soft decoding
#define QPACKETMODEM_SOFT 1

struct qpacketmodem_s {
    // properties
    modem           mod_payload;        // payload modulator/demodulator
    packetizer      p;                  // packet encoder/decoder
    unsigned int    bits_per_symbol;    // modulator bits/symbol
    unsigned int    payload_dec_len;    // number of decoded payload bytes
    unsigned char * payload_enc;        // payload data (encoded bytes)
    unsigned char * payload_mod;        // payload symbols (modulator output, demod input)
    unsigned int    payload_enc_len;    // number of encoded payload bytes
    unsigned int    payload_bit_len;    // number of bits in encoded payload
    unsigned int    payload_mod_len;    // number of symbols in encoded payload
};

// create packet encoder
qpacketmodem qpacketmodem_create()
{
    // allocate memory for main object
    qpacketmodem q = (qpacketmodem) malloc(sizeof(struct qpacketmodem_s));

    // create payload modem (initially QPSK, overridden by properties)
    q->mod_payload = modem_create(LIQUID_MODEM_QPSK);
    q->bits_per_symbol = 2;

    // initial memory allocation for payload
    q->payload_dec_len = 1;
    q->p = packetizer_create(q->payload_dec_len,
                             LIQUID_CRC_NONE,
                             LIQUID_FEC_NONE,
                             LIQUID_FEC_NONE);

    // number of bytes in encoded payload
    q->payload_enc_len = packetizer_get_enc_msg_len(q->p);

    // number of bits in encoded payload
    q->payload_bit_len = 8*q->payload_enc_len;

    // number of symbols in encoded payload
    div_t d = div(q->payload_bit_len, q->bits_per_symbol);
    q->payload_mod_len = d.quot + (d.rem ? 1 : 0);

#if QPACKETMODEM_SOFT
    // soft demodulator uses one byte to represent each soft bit
    q->payload_enc = (unsigned char*) malloc(q->bits_per_symbol*q->payload_mod_len*sizeof(unsigned char));
#else
    // hard demodulator compacts demodulated output bits into array
    q->payload_enc = (unsigned char*) malloc(q->payload_enc_len*sizeof(unsigned char));
#endif

    // set symbol length appropriately
    q->payload_mod_len = q->payload_enc_len * 4;    // for QPSK
    q->payload_mod = (unsigned char*) malloc(q->payload_mod_len*sizeof(unsigned char));

    // return pointer to main object
    return q;
}

// destroy object, freeing all internal arrays
void qpacketmodem_destroy(qpacketmodem _q)
{
    // free objects
    packetizer_destroy(_q->p);
    modem_destroy(_q->mod_payload);

    // free arrays
    free(_q->payload_enc);
    free(_q->payload_mod);
}

// reset object
void qpacketmodem_reset(qpacketmodem _q)
{
    modem_reset(_q->mod_payload);
}

// print object internals
void qpacketmodem_print(qpacketmodem _q)
{
    LOGI("qpacketmodem:\n");
    LOGI("  check             :   %s\n", crc_scheme_str[packetizer_get_crc(_q->p)][1]);
    LOGI("  fec (inner)       :   %s\n", fec_scheme_str[packetizer_get_fec0(_q->p)][1]);
    LOGI("  fec (outer)       :   %s\n", fec_scheme_str[packetizer_get_fec1(_q->p)][1]);
    LOGI("  modulation scheme :   %s\n", modulation_types[modem_get_scheme(_q->mod_payload)].name);
    LOGI("  payload dec len   :   %u\n", _q->payload_dec_len);
    LOGI("  payload enc len   :   %u\n", _q->payload_enc_len);
    LOGI("  payload bit len   :   %u\n", _q->payload_bit_len);
    LOGI("  payload mod len   :   %u\n", _q->payload_mod_len);
}

//
int qpacketmodem_configure(qpacketmodem _q,
                           unsigned int _payload_len,
                           crc_scheme   _check,
                           fec_scheme   _fec0,
                           fec_scheme   _fec1,
                           int          _ms)
{
    // set new decoded message length
    _q->payload_dec_len = _payload_len;

    // recreate modem object and get new bits per symbol
    _q->mod_payload = modem_recreate(_q->mod_payload, _ms);
    _q->bits_per_symbol = modem_get_bps(_q->mod_payload);

    // recreate packetizer object and compute new encoded payload length
    _q->p = packetizer_recreate(_q->p, _q->payload_dec_len, _check, _fec0, _fec1);
    _q->payload_enc_len = packetizer_get_enc_msg_len(_q->p);

    // number of bits in encoded payload
    _q->payload_bit_len = 8*_q->payload_enc_len;

    // number of symbols in encoded payload
    div_t d = div(_q->payload_bit_len, _q->bits_per_symbol);
    _q->payload_mod_len = d.quot + (d.rem ? 1 : 0);

#if QPACKETMODEM_SOFT
    _q->payload_enc = (unsigned char*) realloc(_q->payload_enc,
            _q->bits_per_symbol*_q->payload_mod_len*sizeof(unsigned char));
#else
    _q->payload_enc = (unsigned char*) realloc(_q->payload_enc,
                                               _q->payload_enc_len*sizeof(unsigned char));
#endif

    // reallocate memory for modem symbols
    _q->payload_mod = (unsigned char*) realloc(_q->payload_mod,
                                               _q->payload_mod_len*sizeof(unsigned char));

    return 0;
}

// get length of frame in symbols
unsigned int qpacketmodem_get_frame_len(qpacketmodem _q)
{
    return _q->payload_mod_len;
}

// get payload length (bytes)
unsigned int qpacketmodem_get_payload_len(qpacketmodem _q)
{
    // number of decoded payload bytes
    return _q->payload_dec_len;
}

unsigned int qpacketmodem_get_crc(qpacketmodem _q)
{
    return packetizer_get_crc(_q->p);
}

unsigned int qpacketmodem_get_fec0(qpacketmodem _q)
{
    return packetizer_get_fec0(_q->p);
}

unsigned int qpacketmodem_get_fec1(qpacketmodem _q)
{
    return packetizer_get_fec1(_q->p);
}

unsigned int qpacketmodem_get_modscheme(qpacketmodem _q)
{
    return modem_get_scheme(_q->mod_payload);
}

// encode packet into modulated frame samples
// TODO: include method with just symbol indices? would be useful for
//       non-linear modulation types
void qpacketmodem_encode(qpacketmodem    _q,
                         unsigned char * _payload,
                         float complex * _frame)
{
    unsigned int i;

    // encode payload
    packetizer_encode(_q->p, _payload, _q->payload_enc);

    // clear payload
    memset(_q->payload_mod, 0x00, _q->payload_mod_len);

    // repack 8-bit payload bytes into 'bps'-bit payload symbols
    unsigned int bps = _q->bits_per_symbol;
    unsigned int num_written;
    liquid_repack_bytes(_q->payload_enc,  8,  _q->payload_enc_len,
                        _q->payload_mod, bps, _q->payload_mod_len,
                        &num_written);
    assert(num_written == _q->payload_mod_len);

    for (i=0; i<_q->payload_mod_len; i++)
        modem_modulate(_q->mod_payload, _q->payload_mod[i], &_frame[i]);
}

#if QPACKETMODEM_SOFT
// decode packet from modulated frame samples (soft-decision decoding)
int qpacketmodem_decode(qpacketmodem    _q,
                        float complex * _frame,
                        unsigned char * _payload)
{
    unsigned int i;

    // demodulate and pack bytes into decoder input buffer
    unsigned int sym;
    //memset(_q->payload_enc, 0x00, _q->payload_enc_len*sizeof(unsigned char));
    unsigned int n = 0;
    for (i=0; i<_q->payload_mod_len; i++) {
        // demodulate symbol
        modem_demodulate_soft(_q->mod_payload, _frame[i], &sym, _q->payload_enc+n);
        n += _q->bits_per_symbol;
    }
    //LOGI("received %u bits (expected %u)\n", n, _q->payload_mod_len * _q->bits_per_symbol);
    assert( n == _q->payload_mod_len * _q->bits_per_symbol);

    // decode payload
    int payload_valid = packetizer_decode_soft(_q->p, _q->payload_enc, _payload);

    return payload_valid;
}
#else
// decode packet from modulated frame samples (hard-decision decoding)
int qpacketmodem_decode(qpacketmodem    _q,
                        float complex * _frame,
                        unsigned char * _payload)
{
    unsigned int i;

    // demodulate and pack bytes into decoder input buffer
    unsigned int sym;
    //memset(_q->payload_enc, 0x00, _q->payload_enc_len*sizeof(unsigned char));
    for (i=0; i<_q->payload_mod_len; i++) {
        // demodulate symbol
        modem_demodulate(_q->mod_payload, _frame[i], &sym);

        // pack decoded symbol into array
        liquid_pack_array(_q->payload_enc,
                          _q->payload_enc_len,
                          i * _q->bits_per_symbol,
                          _q->bits_per_symbol,
                          sym);
    }

    // decode payload
    int payload_valid = packetizer_decode(_q->p, _q->payload_enc, _payload);

    return payload_valid;
}
#endif
