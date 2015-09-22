// $Id$
// $(c)$

#include <complex.h>
#include <math.h>
#include "math.isnan.h"

inline int __inline_cisnan(const double complex x) { return (isnan(creal(x))||isnan(cimag(x))); }
inline int __inline_cisnanf(const float complex x) { return (isnanf(crealf(x))||isnanf(cimagf(x))); }
inline int __inline_cisnanl(const long double complex x) { return (isnanl(creall(x))||isnanl(cimagl(x))); }

