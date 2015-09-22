// $Id$
// $(c)$

#include "liquid.fleetcents.h"

inline int __inline_cisnan(double complex x) {
  return (isnan(creal(x))||isnan(cimag(x)));
}

inline int __inline_cisnanf(float complex x) {
  return (isnan(creal(x))||isnan(cimag(x)));
}

inline int __inline_cisnanl(long double complex x) {
  return (isnan(creal(x))||isnan(cimag(x)));
}
