// $Id$
// $(c)$

#ifndef __LIQUID_FLEETCENTS_H
#define __LIQUID_FLEETCENTS_H

#include <tgmath.h>
#include <math.h>

int __inline_cisnan(double complex x);
int __inline_cisnanf(float complex x);
int __inline_cisnanl(long double complex x);

#undef isnan
#define isnan(x) \
	((sizeof (x) == sizeof (long double complex)) ? __inline_cisnanl(x)		\
	: ((sizeof (x) == sizeof (double complex)) ? __inline_cisnan(x)		\
	: ((sizeof (x) == sizeof (float complex)) ? __inline_cisnanf(x)		\
	: ((sizeof (x) == sizeof (float)) ? isnanf(x)		\
    : (sizeof (x) == sizeof (double)) ? isnan(x)	\
    : __isnanl(x)))))

#endif