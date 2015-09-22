// FFT code taken from https://gist.github.com/kristianlm/4df96321d771257aab32
// code from http://paulbourke.net/miscellaneous/dft/
// help from http://www.codeproject.com/Articles/9388/How-to-implement-the-FFT-algorithm

#include <math.h>
#include "fft.h"

/*
  This computes an in-place cmplx-to-cmplx FFT
  x and y are the real and imaginary arrays of 2^m points.
  dir =  1 gives forward transform
  dir = -1 gives reverse transform
*/

short fft(short int dir, long m, struct cmplx *buffer)
{
  long n,i,i1,j,k,i2,l,l1,l2;
  double c1,c2,tx,ty,t1,t2,u1,u2,z;

  /* Calculate the number of points */
  n = 1;
  for (i=0;i<m;i++)
    n *= 2;

  /* Do the bit reversal */
  i2 = n >> 1;
  j = 0;
  for (i=0;i<n-1;i++) {
    if (i < j) {
      tx = buffer[i].real;
      ty = buffer[i].imag;
      buffer[i].real = buffer[j].real;
      buffer[i].imag = buffer[j].imag;
      buffer[j].real = tx;
      buffer[j].imag = ty;
    }
    k = i2;
    while (k <= j) {
      j -= k;
      k >>= 1;
    }
    j += k;
  }

  /* Compute the FFT */
  c1 = -1.0;
  c2 = 0.0;
  l2 = 1;
  for (l=0;l<m;l++) {
    l1 = l2;
    l2 <<= 1;
    u1 = 1.0;
    u2 = 0.0;
    for (j=0;j<l1;j++) {
      for (i=j;i<n;i+=l2) {
        i1 = i + l1;
        t1 = u1 * buffer[i1].real - u2 * buffer[i1].imag;
        t2 = u1 * buffer[i1].imag + u2 * buffer[i1].real;
        buffer[i1].real = buffer[i].real - t1;
        buffer[i1].imag = buffer[i].imag - t2;
        buffer[i].real += t1;
        buffer[i].imag += t2;
      }
      z =  u1 * c1 - u2 * c2;
      u2 = u1 * c2 + u2 * c1;
      u1 = z;
    }
    c2 = sqrt((1.0 - c1) / 2.0);
    if (dir == 1)
      c2 = -c2;
    c1 = sqrt((1.0 + c1) / 2.0);
  }

  /* Scaling for forward transform */
  if (dir == 1) {
    for (i=0;i<n;i++) {
      buffer[i].real /= n;
      buffer[i].imag /= n;
    }
  }

  return(1);
}
