// $Id$
// $(c)$

#ifndef __MATH_ISNAN_H
#define __MATH_ISNAN_H

#include <complex.h>
#include <math.h>

inline int __inline_cisnan(const double complex x);
inline int __inline_cisnanf(const float complex x);
inlien int __inline_cisnanl(const long double complex x);

#if !defined(__clang__) && (!defined(__GNUC__) || \
    (defined(__GNUC__) && ((__GNUC__ < 4) || \
                      (__GNUC__ == 4 && (__GNUC_MINOR__ < 9)))))

#undef isnan
#define isnan(x) \
  (((sizeof(x) == sizeof(long double complex)) && (sizeof(creall(x)) == sizeof(long double))) ? __inline_cisnanl(x)   \
  : (((sizeof(x) == sizeof(double complex)) && (sizeof(creal(x)) == sizeof(double))) ? __inline_cisnan(x)   \
  : (((sizeof(x) == sizeof(float complex)) && (sizeof(crealf(x)) == sizeof(float))) ? __inline_cisnanf(x) \
  : ((sizeof(x) == sizeof(float)) ? __inline_isnanf(x) \
  : ((sizeof(x) == sizeof(double)) ? __inline_isnan(x) \
  : __inline_isnanl(x))))))


#endif

#endif
