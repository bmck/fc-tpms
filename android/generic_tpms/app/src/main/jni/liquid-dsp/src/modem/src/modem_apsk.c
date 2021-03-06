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
// modem_apsk.c
//

// create an apsk (amplitude/phase-shift keying) modem object
MODEM() MODEM(_create_apsk)(unsigned int _bits_per_symbol)
{
    // pointer to APSK definition container
    struct liquid_apsk_s * apskdef = NULL;

    switch (_bits_per_symbol) {
    case 2: apskdef = &liquid_apsk4;    break;
    case 3: apskdef = &liquid_apsk8;    break;
    case 4: apskdef = &liquid_apsk16;   break;
    case 5: apskdef = &liquid_apsk32;   break;
    case 6: apskdef = &liquid_apsk64;   break;
    case 7: apskdef = &liquid_apsk128;  break;
    case 8: apskdef = &liquid_apsk256;  break;
    default:
        LOGE("error: modem_create_apsk(), unsupported modulation level (%u)\n",
                _bits_per_symbol);
        exit(1);
    }

    MODEM() q = (MODEM()) malloc( sizeof(struct MODEM(_s)) );
    q->scheme = apskdef->scheme;
    MODEM(_init)(q, _bits_per_symbol);

    // set APSK internals
    unsigned int i;
    q->data.apsk.num_levels = apskdef->num_levels;
    for (i=0; i<q->data.apsk.num_levels; i++) {
        q->data.apsk.p[i]   = apskdef->p[i];
        q->data.apsk.r[i]   = apskdef->r[i];
        q->data.apsk.phi[i] = apskdef->phi[i];
    }

    // radius slicer
    for (i=0; i<q->data.apsk.num_levels-1; i++)
        q->data.apsk.r_slicer[i] = apskdef->r_slicer[i];

    // copy symbol map
    q->data.apsk.map = (unsigned char *) malloc(q->M*sizeof(unsigned char));
    memmove(q->data.apsk.map, apskdef->map, q->M*sizeof(unsigned char));

    // set modulation/demodulation function pointers
    q->modulate_func = &MODEM(_modulate_apsk);
    q->demodulate_func = &MODEM(_demodulate_apsk);

    // initialize soft-demodulation look-up table
    switch (q->m) {
    case 2: MODEM(_demodsoft_gentab)(q, 3); break;
    case 3: MODEM(_demodsoft_gentab)(q, 3); break;
    case 4: MODEM(_demodsoft_gentab)(q, 4); break;
    case 5: MODEM(_demodsoft_gentab)(q, 4); break;
    case 6: MODEM(_demodsoft_gentab)(q, 4); break;
    case 7: MODEM(_demodsoft_gentab)(q, 5); break;
    case 8: MODEM(_demodsoft_gentab)(q, 5); break;
    default:;
    }

    // initialize symbol map
    q->symbol_map = (TC*)malloc(q->M*sizeof(TC));
    MODEM(_init_map)(q);
    q->modulate_using_map = 1;

    // reset modem and return
    MODEM(_reset)(q);
    return q;
}

// modulate APSK
void MODEM(_modulate_apsk)(MODEM()      _q,
                           unsigned int _sym_in,
                           TC *         _y)
{
    if (_sym_in >= _q->M) {
        LOGE("error: modem_modulate_apsk(), input symbol exceeds maximum\n");
        return;
    }

    // map input symbol to constellation symbol
    unsigned int i;
    unsigned int s = _q->data.apsk.map[_sym_in];

    // determine in which level the symbol is located
    unsigned int p=0;   // level
    unsigned int t=0;   // accumulated number of points per level
    for (i=0; i<_q->data.apsk.num_levels; i++) {
        if (s < t + _q->data.apsk.p[i]) {
            p = i;
            break;
        }
        t += _q->data.apsk.p[i];
    }
    unsigned int s0 = s - t;
    unsigned int s1 = _q->data.apsk.p[p];

#if 0
    LOGI("  s : %3u -> %3u in level %3u (t = %3u) [symbol %3u / %3u]\n", _sym_in, s, p, t, s0,s1);
#endif

    // map symbol to constellation point (radius, angle)
    T r = _q->data.apsk.r[p];
    T phi = _q->data.apsk.phi[p] + (T)(s0)*2.0f*M_PI / (T)(s1);

    // compute output symbol
    *_y = r * liquid_cexpjf(phi);
}

// demodulate APSK
void MODEM(_demodulate_apsk)(MODEM()        _q,
                             TC             _x,
                             unsigned int * _sym_out)
{
    // compute amplitude
    T r = cabsf(_x);

    // determine which ring to demodulate with
    unsigned int i, p=0;
    for (i=0; i<_q->data.apsk.num_levels-1; i++) {
        if (r < _q->data.apsk.r_slicer[i]) {
            p = i;
            break;
        } else {
            p = _q->data.apsk.num_levels-1;
        }
    }

    // find closest point in ring
    T theta = cargf(_x);
    if (theta < 0.0f) theta += 2.0f*M_PI;
    T dphi = 2.0f*M_PI / (T) _q->data.apsk.p[p];
    unsigned int s_hat=0;
    T i_hat = (theta - _q->data.apsk.phi[p]) / dphi;
    s_hat = roundf(i_hat);      // compute symbol (closest angle)
    s_hat %= _q->data.apsk.p[p];   // ensure symbol is in range
    //LOGI("          i_hat : %12.8f (%3u)\n", i_hat, s_hat);

    // accumulate symbol points
    for (i=0; i<p; i++)
        s_hat += _q->data.apsk.p[i];
    //assert(s_hat < _q->M);

    // reverse symbol mapping
    unsigned int s_prime=0;
    for (i=0; i<_q->M; i++) {
        if ( _q->data.apsk.map[i] == s_hat) {
            s_prime = i;
            break;
        }
    }

#if 0
    LOGI("              x : %12.8f + j*%12.8f\n", crealf(_x), cimagf(_x));
    LOGI("              p : %3u\n", p);
    LOGI("          theta : %12.8f\n", theta);
    LOGI("           dmin : %12.8f\n", dmin);
    LOGI("              s : %3u > %3u\n", s_hat, s_prime);
#endif

    *_sym_out = s_prime;

    // re-modulate symbol and store state
    MODEM(_modulate)(_q, s_prime, &_q->x_hat);
    _q->r = _x;
}

