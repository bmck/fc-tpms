// $Id$
// $(c)$

#ifndef __UNIVERSAL_DEFINES_H
#define __UNIVERSAL_DEFINES_H

#include "log_macros.h"

#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

#define RTN_OK 0 // finished working on this buffer, valid state so far & keep working
#define RTN_FINISHED 1 // found end of valid packet
#define RTN_BAD 2 // found invalid
#define RTN_BUG 999

#define IQ_SAMPLES_PER_BLOCK 256
#define BITS_PER_FREESCALE_MSG 69
#define SAMPLE_RATE 1.024e6

#include <math.h>
#define PI M_PI

typedef enum { false, true } bool;
struct cmplx { double real, imag; } ;
struct iqpair { unsigned char i, q; } ;

#endif