// $Id$
// $(c)$

/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
   Command line: /www/usr/fisher/helpers/mkfilter -Bu -Bp -o 10 -a 1.7089843750e-02 2.1972656250e-02 -Z 1.4648437500e-02 -l */
// see http://www-users.cs.york.ac.uk/~fisher/mkfilter/trad.html
// see also http://www.cardinalpeak.com/blog/a-c-class-to-implement-low-pass-high-pass-and-band-pass-filters/

#ifndef __BANDPASS_FILTER_H
#define __BANDPASS_FILTER_H

#include "universal_defines.h"

#define NPOLES 22
#define NZEROS 22
#define GAIN 6.547978948e+02

void bpffilter(struct iqpair i, struct iqpair *o);

#endif
