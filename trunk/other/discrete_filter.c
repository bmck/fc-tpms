// $Id$
// $(c)$

// see http://www.cardinalpeak.com/blog/a-c-class-to-implement-low-pass-high-pass-and-band-pass-filters/

#include "./discrete_filter.h"
#define ECODE(f,x) { f->m_error_flag = x; return;}

// Handles LPF and HPF case
void init_filter(struct filter *fltr, enum filterType filt_t, int num_taps, double Fs, double Fx) {
  fltr->m_error_flag = 0;
  fltr->m_filt_t = filt_t;
  fltr->m_num_taps = num_taps;
  fltr->m_Fs = Fs;
  fltr->m_Fx = Fx;
  fltr->m_lambda = M_PI * Fx / (Fs/2);

  if( Fs <= 0 ) ECODE(fltr, -1);
  if( Fx <= 0 || Fx >= Fs/2 ) ECODE(fltr, -2);
  if( fltr->m_num_taps <= 0 || fltr->m_num_taps > MAX_NUM_FILTER_TAPS ) ECODE(fltr, -3);

  fltr->m_taps = fltr->m_sr = NULL;
  fltr->m_taps = (double*)malloc( fltr->m_num_taps * sizeof(double) );
  fltr->m_sr = (double*)malloc( fltr->m_num_taps * sizeof(double) );
  if( fltr->m_taps == NULL || fltr->m_sr == NULL ) ECODE(fltr, -4);

  common_init_filter(fltr);

  if( fltr->m_filt_t == LPF ) designLPF(fltr);
  else if( fltr->m_filt_t == HPF ) designHPF(fltr);
  else ECODE(fltr, -5);

  return;
}

void init_LPfilter(struct filter *fltr, enum filterType filt_t, int num_taps, double Fs, double Fx) {
  init_filter(fltr, filt_t, num_taps, Fs, Fx);
}

void init_HPfilter(struct filter *fltr, enum filterType filt_t, int num_taps, double Fs, double Fx) {
  init_filter(fltr, filt_t, num_taps, Fs, Fx);
}

void init_BPfilter(struct filter *fltr, enum filterType filt_t, int num_taps, double Fs, double Fl, double Fu) {
  fltr->m_error_flag = 0;
  fltr->m_filt_t = filt_t;
  fltr->m_num_taps = num_taps;
  fltr->m_Fs = Fs;
  fltr->m_Fx = Fl;
  fltr->m_Fu = Fu;
  fltr->m_lambda = M_PI * Fl / (Fs/2);
  fltr->m_phi = M_PI * Fu / (Fs/2);

  if( Fs <= 0 ) ECODE(fltr, -10);
  if( Fl >= Fu ) ECODE(fltr, -11);
  if( Fl <= 0 || Fl >= Fs/2 ) ECODE(fltr, -12);
  if( Fu <= 0 || Fu >= Fs/2 ) ECODE(fltr, -13);
  if( fltr->m_num_taps <= 0 || fltr->m_num_taps > MAX_NUM_FILTER_TAPS ) ECODE(fltr, -14);

  fltr->m_taps = fltr->m_sr = NULL;
  fltr->m_taps = (double*)malloc( fltr->m_num_taps * sizeof(double) );
  fltr->m_sr = (double*)malloc( fltr->m_num_taps * sizeof(double) );
  if( fltr->m_taps == NULL || fltr->m_sr == NULL ) ECODE(fltr, -15);

  common_init_filter(fltr);

  if( fltr->m_filt_t == BPF ) designBPF(fltr);
  else ECODE(fltr, -16);

  return;
}

void common_init_filter(struct filter *fltr) {
  int i;

  if( fltr->m_error_flag != 0 ) return;

  for(i = 0; i < fltr->m_num_taps; i++) fltr->m_sr[i] = 0;

  return;
}

void deinit_filter(struct filter *fltr) {
  if( fltr->m_taps != NULL ) free( fltr->m_taps );
  if( fltr->m_sr != NULL ) free( fltr->m_sr );
}

void designLPF(struct filter *fltr) {
  int n;
  double mm;

  for(n = 0; n < fltr->m_num_taps; n++){
    mm = n - (fltr->m_num_taps - 1.0) / 2.0;
    if( mm == 0.0 ) fltr->m_taps[n] = fltr->m_lambda / M_PI;
    else fltr->m_taps[n] = sin( mm * fltr->m_lambda ) / (mm * M_PI);
  }

  return;
}

void designHPF(struct filter *fltr) {
  int n;
  double mm;

  for(n = 0; n < fltr->m_num_taps; n++){
    mm = n - (fltr->m_num_taps - 1.0) / 2.0;
    if( mm == 0.0 ) fltr->m_taps[n] = 1.0 - fltr->m_lambda / M_PI;
    else fltr->m_taps[n] = -sin( mm * fltr->m_lambda ) / (mm * M_PI);
  }

  return;
}

void designBPF(struct filter *fltr) {
  int n;
  double mm;

  for(n = 0; n < fltr->m_num_taps; n++){
    mm = n - (fltr->m_num_taps - 1.0) / 2.0;
    if( mm == 0.0 ) fltr->m_taps[n] = (fltr->m_phi - fltr->m_lambda) / M_PI;
    else fltr->m_taps[n] = (   sin( mm * fltr->m_phi ) -
                         sin( mm * fltr->m_lambda )   ) / (mm * M_PI);
  }

  return;
}

double do_sample(struct filter *fltr, double data_sample) {
  int i;
  double result;

  if( fltr->m_error_flag != 0 ) return(0);

  for(i = fltr->m_num_taps - 1; i >= 1; i--){
    fltr->m_sr[i] = fltr->m_sr[i-1];
  }
  fltr->m_sr[0] = data_sample;

  result = 0;
  for(i = 0; i < fltr->m_num_taps; i++) result += fltr->m_sr[i] * fltr->m_taps[i];

  return result;

}

int get_error_flag(struct filter *fltr) {
  return fltr->m_error_flag;
}

void get_taps(struct filter *fltr,  double *taps) {
  int i;

  if( fltr->m_error_flag != 0 ) return;

  for(i = 0; i < fltr->m_num_taps; i++) taps[i] = fltr->m_taps[i];

  return;
}

int write_taps_to_file(struct filter *fltr, char *filename) {
  FILE *fd;

  if( fltr->m_error_flag != 0 ) return -1;

  int i;
  fd = fopen(filename, "w");
  if( fd == NULL ) return -1;

  fprintf(fd, "%d\n", fltr->m_num_taps);
  for(i = 0; i < fltr->m_num_taps; i++){
    fprintf(fd, "%15.6f\n", fltr->m_taps[i]);
  }
  fclose(fd);

  return 0;
}

// Output the magnitude of the frequency response in dB
#define NP 1000
int write_freqres_to_file(struct filter *fltr, char *filename) {
  FILE *fd;
  int i, k;
  double w, dw;
  double y_r[NP], y_i[NP], y_mag[NP];
  double mag_max = -1;
  double tmp_d;

  if( fltr->m_error_flag != 0 ) return -1;

  dw = M_PI / (NP - 1.0);
  for(i = 0; i < NP; i++){
    w = i*dw;
    y_r[i] = y_i[i] = 0;
    for(k = 0; k < fltr->m_num_taps; k++){
      y_r[i] += fltr->m_taps[k] * cos(k * w);
      y_i[i] -= fltr->m_taps[k] * sin(k * w);
    }
  }

  for(i = 0; i < NP; i++){
    y_mag[i] = sqrt( y_r[i]*y_r[i] + y_i[i]*y_i[i] );
    if( y_mag[i] > mag_max ) mag_max = y_mag[i];
  }

  if( mag_max <= 0.0 ) return -2;

  fd = fopen(filename, "w");
  if( fd == NULL ) return -3;

  for(i = 0; i < NP; i++){
    w = i*dw;
    if( y_mag[i] == 0 ) tmp_d = -100;
    else{
      tmp_d = 20 * log10( y_mag[i] / mag_max );
      if( tmp_d < -100 ) tmp_d = -100;
    }
    fprintf(fd, "%10.6e %10.6e\n", w * (fltr->m_Fs/2)/M_PI, tmp_d);
  }

  fclose(fd);
  return 0;
}
#undef NP