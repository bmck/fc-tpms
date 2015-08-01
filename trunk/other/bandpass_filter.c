// $Id$
// $(c)$

/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
   Command line: /www/usr/fisher/helpers/mkfilter -Bu -Bp -o 10 -a 1.7089843750e-02 2.1972656250e-02 -Z 1.4648437500e-02 -l */
// see http://www-users.cs.york.ac.uk/~fisher/mkfilter/trad.html
// see also http://www.cardinalpeak.com/blog/a-c-class-to-implement-low-pass-high-pass-and-band-pass-filters/

#include "./bandpass_filter.h"

static float xv[NPOLES+1], yv[NPOLES+1];

unsigned char bpffilter(unsigned char i) {
  float ii; ii = (float) i;
  float o;

  xv[0] = xv[1]; xv[1] = xv[2]; xv[2] = xv[3]; xv[3] = xv[4]; xv[4] = xv[5]; xv[5] = xv[6]; xv[6] = xv[7]; xv[7] = xv[8]; xv[8] = xv[9]; xv[9] = xv[10]; xv[10] = xv[11]; xv[11] = xv[12]; xv[12] = xv[13]; xv[13] = xv[14]; xv[14] = xv[15]; xv[15] = xv[16]; xv[16] = xv[17]; xv[17] = xv[18]; xv[18] = xv[19]; xv[19] = xv[20]; xv[20] = xv[21]; xv[21] = xv[22];
  xv[22] = ii / GAIN;
  yv[0] = yv[1]; yv[1] = yv[2]; yv[2] = yv[3]; yv[3] = yv[4]; yv[4] = yv[5]; yv[5] = yv[6]; yv[6] = yv[7]; yv[7] = yv[8]; yv[8] = yv[9]; yv[9] = yv[10]; yv[10] = yv[11]; yv[11] = yv[12]; yv[12] = yv[13]; yv[13] = yv[14]; yv[14] = yv[15]; yv[15] = yv[16]; yv[16] = yv[17]; yv[17] = yv[18]; yv[18] = yv[19]; yv[19] = yv[20]; yv[20] = yv[21]; yv[21] = yv[22];
  yv[22] =   (xv[0] + xv[22]) -   1.9915348289 * (xv[1] + xv[21]) - 9 * (xv[2] + xv[20])
             +  19.9153482890 * (xv[3] + xv[19]) + 35 * (xv[4] + xv[18]) -  89.6190673020 * (xv[5] + xv[17])
             - 75 * (xv[6] + xv[16]) + 238.9841794700 * (xv[7] + xv[15]) + 90 * (xv[8] + xv[14])
             - 418.2223140800 * (xv[9] + xv[13]) - 42 * (xv[10] + xv[12]) + 501.8667768900 * xv[11]
             + ( -0.0000000000 * yv[0]) + ( -0.0000000000 * yv[1])
             + ( -0.8219069486 * yv[2]) + ( 16.4764331130 * yv[3])
             + (-157.0151910200 * yv[4]) + (945.7825679300 * yv[5])
             + (-4038.5125078000 * yv[6]) + (12994.3576690000 * yv[7])
             + (-32690.3770130000 * yv[8]) + (65844.0157530000 * yv[9])
             + (-107839.2894900000 * yv[10]) + (145031.9953100000 * yv[11])
             + (-161044.2264300000 * yv[12]) + (147904.5938500000 * yv[13])
             + (-112153.4645600000 * yv[14]) + (69834.4647320000 * yv[15])
             + (-35358.2881780000 * yv[16]) + (14333.2242650000 * yv[17])
             + (-4542.8485393000 * yv[18]) + (1084.9652711000 * yv[19])
             + (-183.6893023000 * yv[20]) + ( 19.6572608240 * yv[21]);

  o = yv[22];
  if yv[22] < 0.0 o = 0.0;
  if yv[22] > 511.0 o = 511.0;

  return ((unsigned char) o);
}
