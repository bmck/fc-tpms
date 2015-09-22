// $Id$
// $(c)$

/* Digital filter designed by mkfilter/mkshape/gencode   A.J. Fisher
   Command line: /www/usr/fisher/helpers/mkfilter -Bu -Bp -o 10 -a 1.7089843750e-02 2.1972656250e-02 -Z 1.4648437500e-02 -l */
// see http://www-users.cs.york.ac.uk/~fisher/mkfilter/trad.html
// see also http://www.cardinalpeak.com/blog/a-c-class-to-implement-low-pass-high-pass-and-band-pass-filters/

#include "./cplx_bandpass_filter.h"

struct iqdbls { double i, q; } ;
static struct iqdbls xv[NPOLES+1], yv[NPOLES+1];

void bpffilter(struct iqpair in, struct iqpair *out) {
  double ii, qq;

  xv[0].i = xv[1].i; xv[1].i = xv[2].i; xv[2].i = xv[3].i; xv[3].i = xv[4].i; xv[4].i = xv[5].i; xv[5].i = xv[6].i; xv[6].i = xv[7].i; xv[7].i = xv[8].i; xv[8].i = xv[9].i; xv[9].i = xv[10].i; xv[10].i = xv[11].i; xv[11].i = xv[12].i; xv[12].i = xv[13].i; xv[13].i = xv[14].i; xv[14].i = xv[15].i; xv[15].i = xv[16].i; xv[16].i = xv[17].i; xv[17].i = xv[18].i; xv[18].i = xv[19].i; xv[19].i = xv[20].i; xv[20].i = xv[21].i; xv[21].i = xv[22].i;
  xv[0].q = xv[1].q; xv[1].q = xv[2].q; xv[2].q = xv[3].q; xv[3].q = xv[4].q; xv[4].q = xv[5].q; xv[5].q = xv[6].q; xv[6].q = xv[7].q; xv[7].q = xv[8].q; xv[8].q = xv[9].q; xv[9].q = xv[10].q; xv[10].q = xv[11].q; xv[11].q = xv[12].q; xv[12].q = xv[13].q; xv[13].q = xv[14].q; xv[14].q = xv[15].q; xv[15].q = xv[16].q; xv[16].q = xv[17].q; xv[17].q = xv[18].q; xv[18].q = xv[19].q; xv[19].q = xv[20].q; xv[20].q = xv[21].q; xv[21].q = xv[22].q;
  xv[22].i = in.i / GAIN; xv[22].q = in.q / GAIN;

  yv[0].i = yv[1].i; yv[1].i = yv[2].i; yv[2].i = yv[3].i; yv[3].i = yv[4].i; yv[4].i = yv[5].i; yv[5].i = yv[6].i; yv[6].i = yv[7].i; yv[7].i = yv[8].i; yv[8].i = yv[9].i; yv[9].i = yv[10].i; yv[10].i = yv[11].i; yv[11].i = yv[12].i; yv[12].i = yv[13].i; yv[13].i = yv[14].i; yv[14].i = yv[15].i; yv[15].i = yv[16].i; yv[16].i = yv[17].i; yv[17].i = yv[18].i; yv[18].i = yv[19].i; yv[19].i = yv[20].i; yv[20].i = yv[21].i; yv[21].i = yv[22].i;
  yv[0].q = yv[1].q; yv[1].q = yv[2].q; yv[2].q = yv[3].q; yv[3].q = yv[4].q; yv[4].q = yv[5].q; yv[5].q = yv[6].q; yv[6].q = yv[7].q; yv[7].q = yv[8].q; yv[8].q = yv[9].q; yv[9].q = yv[10].q; yv[10].q = yv[11].q; yv[11].q = yv[12].q; yv[12].q = yv[13].q; yv[13].q = yv[14].q; yv[14].q = yv[15].q; yv[15].q = yv[16].q; yv[16].q = yv[17].q; yv[17].q = yv[18].q; yv[18].q = yv[19].q; yv[19].q = yv[20].q; yv[20].q = yv[21].q; yv[21].q = yv[22].q;

  yv[22].i =   (xv[0].i + xv[22].i) -   1.9915348289 * (xv[1].i + xv[21].i) - 9 * (xv[2].i + xv[20].i)
             +  19.9153482890 * (xv[3].i + xv[19].i) + 35 * (xv[4].i + xv[18].i) -  89.6190673020 * (xv[5].i + xv[17].i)
             - 75 * (xv[6].i + xv[16].i) + 238.9841794700 * (xv[7].i + xv[15].i) + 90 * (xv[8].i + xv[14].i)
             - 418.2223140800 * (xv[9].i + xv[13].i) - 42 * (xv[10].i + xv[12].i) + 501.8667768900 * xv[11].i
             + ( -0.0000000000 * yv[0].i) + ( -0.0000000000 * yv[1].i)
             + ( -0.8219069486 * yv[2].i) + ( 16.4764331130 * yv[3].i)
             + (-157.0151910200 * yv[4].i) + (945.7825679300 * yv[5].i)
             + (-4038.5125078000 * yv[6].i) + (12994.3576690000 * yv[7].i)
             + (-32690.3770130000 * yv[8].i) + (65844.0157530000 * yv[9].i)
             + (-107839.2894900000 * yv[10].i) + (145031.9953100000 * yv[11].i)
             + (-161044.2264300000 * yv[12].i) + (147904.5938500000 * yv[13].i)
             + (-112153.4645600000 * yv[14].i) + (69834.4647320000 * yv[15].i)
             + (-35358.2881780000 * yv[16].i) + (14333.2242650000 * yv[17].i)
             + (-4542.8485393000 * yv[18].i) + (1084.9652711000 * yv[19].i)
             + (-183.6893023000 * yv[20].i) + ( 19.6572608240 * yv[21].i);

  yv[22].q =   (xv[0].q + xv[22].q) -   1.9915348289 * (xv[1].q + xv[21].q) - 9 * (xv[2].q + xv[20].q)
             +  19.9153482890 * (xv[3].q + xv[19].q) + 35 * (xv[4].q + xv[18].q) -  89.6190673020 * (xv[5].q + xv[17].q)
             - 75 * (xv[6].q + xv[16].q) + 238.9841794700 * (xv[7].q + xv[15].q) + 90 * (xv[8].q + xv[14].q)
             - 418.2223140800 * (xv[9].q + xv[13].q) - 42 * (xv[10].q + xv[12].q) + 501.8667768900 * xv[11].q
             + ( -0.0000000000 * yv[0].q) + ( -0.0000000000 * yv[1].q)
             + ( -0.8219069486 * yv[2].q) + ( 16.4764331130 * yv[3].q)
             + (-157.0151910200 * yv[4].q) + (945.7825679300 * yv[5].q)
             + (-4038.5125078000 * yv[6].q) + (12994.3576690000 * yv[7].q)
             + (-32690.3770130000 * yv[8].q) + (65844.0157530000 * yv[9].q)
             + (-107839.2894900000 * yv[10].q) + (145031.9953100000 * yv[11].q)
             + (-161044.2264300000 * yv[12].q) + (147904.5938500000 * yv[13].q)
             + (-112153.4645600000 * yv[14].q) + (69834.4647320000 * yv[15].q)
             + (-35358.2881780000 * yv[16].q) + (14333.2242650000 * yv[17].q)
             + (-4542.8485393000 * yv[18].q) + (1084.9652711000 * yv[19].q)
             + (-183.6893023000 * yv[20].q) + ( 19.6572608240 * yv[21].q);

  LOGI("yv[22].i = %lf, yv[22].q = %lf\n", yv[22].i, yv[22].q);
  ii = MIN(MAX(yv[22].i, 0.0), 511.0);
  qq = MIN(MAX(yv[22].q, 0.0), 511.0);

  out->i = (unsigned char)ii;
  out->q = (unsigned char)qq;
  LOGI("out->i = 0x%X, out->q = 0x%X\n", out->i, out->q);

  return;
}
