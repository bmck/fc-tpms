// $Id$
// $(c)$

#ifndef __FFT_H
#define __FFT_H

#include "universal_defines.h"
#include <complex.h>

template <> short fft<T>(short int dir, long m, float complex *buffer);

#endif