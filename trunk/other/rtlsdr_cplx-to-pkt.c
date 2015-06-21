// $Id$
// $(c)$

// Adapted from ...
//
// IQ file converter for Software Defined Radio Programs rtl_sdr, gqrx
// from gqrx/gnuradio .cfile playback format -- complex64
// to binary packet as string
//
// compile on Linux or Mac with:  gcc ./rtlsdr_cplx-to-pkt.c -o rtlsdr_cplx-to-pkt
//
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>

static int bits_per_pkt = 69;
static char bit_vals[69];

long compute_ccindex_contain_sample(long samp, long *cc, long cc_length) {
  long i;
  for (i = 1; i < cc_length ; i++) {
    if ((cc[i]-cc[0]) > samp) {
      return i;
    }
  }
  return i;
}

unsigned long get_dec_address_val( /*char *bit_vals, long num_bits*/ ) {
  unsigned long addr;
  int j;
  addr = 0; for (j = 2; j <= 29; j++) { addr = addr * 2 + ((bit_vals[j]=='1') ? 1 : 0); }
  return addr;
}

char *get_hex_address_str( /*char *bit_vals, long num_bits,*/ char *returned_url) {
  char *addr_str = malloc(sizeof(char) * 10);
  sprintf(addr_str, "%lX", get_dec_address_val( /*bit_vals, num_bits */));
  return (returned_url = addr_str);
}

long get_temp_c( /*char *bit_vals, long num_bits*/ ) {
  long tempC;
  int j;
  tempC = 0; for (j = 52; j <= 59; j++) { tempC = tempC * 2 + ((bit_vals[j]=='1') ? 1 : 0); }
  return (tempC - 40);
}

long get_temp_f( /*char *bit_vals, long num_bits*/ ) {
  long tempC;
  tempC = get_temp_c( /*bit_vals, num_bits */);
  return ((double)(tempC*1.8))+32;
}

double get_pressure_kpa( /*char *bit_vals, long num_bits*/ ) {
  long press;
  int j;
  press = 0L; for (j = 36; j <= 43; j++) { press = press * 2 + (bit_vals[j]=='1' ? 1 : 0); };
  return ((double)press*2.5-100.0);
}

double get_pressure_psi( /*char *bit_vals, long num_bits */ ) {
  return get_pressure_kpa( /*bit_vals, num_bits */) * 0.145037738;
}

char *get_url( /*long addr, long press, long temp,*/ char *returned_url) {
  char *url = malloc(sizeof(char) * 2048);
  returned_url = url;

  time_t rawtime;
  struct tm * ptm;
  time ( &rawtime );
  ptm = gmtime ( &rawtime );

  unsigned long addr = get_dec_address_val();
  long press = get_pressure_psi();
  long temp = get_temp_c();

  sprintf(url, "http://server11288.baremetalcloud.com/tire_samples/create?tire_sample[sensor_id]=%ld&tire_sample[receiver_id]=%d&tire_sample[value]=%ld&tire_sample[sample_time]=%d-%02d-%02d%%20%02d:%02d:%02d",
    addr, 4, press, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  return returned_url;
}


/*char **/
void get_pkt( const char *infilename /*float sample_rate, const char *infilename, char *bits*/ )
{
  typedef enum { false, true } bool;
  float sample_rate = 2.048e6;

  unsigned long infilesize;
  FILE *infile = fopen(infilename, "rb");
  if (infile) {
    fseek(infile, 0L, SEEK_END);
    infilesize = (unsigned long) ftell(infile);
  }
  else {
    exit(1);
  }

  float v1, v2;
  long max_f = ceil((double)infilesize/8.0), j, k;
  double x[max_f], i[max_f], t[max_f];
  long pos_peak_samples[max_f], neg_samples[max_f];
  long ai, bi;
  ai = 0L; bi = 0L; j = 0L;

  rewind(infile);

  while (!feof(infile) && j < max_f) {
    fread((void*)(&v1), sizeof(v1), 1, infile);
    t[j] = (double)j/sample_rate;
    x[j] = (double)v1;
    fread((void*)(&v2), sizeof(v2), 1, infile);
    i[j] = v2;

    if (x[j] > 0.995) {
      pos_peak_samples[ai] = j; ai++;
    }
    else if (x[j] < 0.0) {
      neg_samples[bi] = j; bi++;
    }
    j++;
  }
  fclose(infile);

  long pos_peak_samples_length, neg_samples_length;
  pos_peak_samples_length = ai;
  neg_samples_length = bi;
  // # "pos_peak_samples" is the set of samples within the file at which the signal amplitude is approximately 1.0
  // # "neg_samples" is the set of samples within the file at which the signal amplitude is less than 0.0


  // # pos_peak_immed_follow_neg_samples is an array of transition points betw high freq and low freq.
  // # This array does not show whether it is a transition hi->lo or lo->hi, but that can be determined by
  // # sampling sufficiently far away from the transition.
  // # This transition point is not exactly accurate; the positive error will always be on the side with the high freq,
  // # and the error with average to be 50% of the high freq period.
  long pos_peak_immed_follow_neg_samples[pos_peak_samples_length], cci;
  long pos_sample = pos_peak_samples[0], prev_pos_sample;

  long cindex;
  for (cci = 0L, cindex = 1L; cindex < pos_peak_samples_length; cindex++) {
    prev_pos_sample = pos_sample;
    pos_sample = pos_peak_samples[cindex];
    long tmp_index;
    bool flag = false;
    for (tmp_index = prev_pos_sample + 1; tmp_index < pos_sample; tmp_index++) {
      int neg_samples_index;
      if (flag) break;
      for (neg_samples_index = 0; neg_samples_index < neg_samples_length; neg_samples_index++) {
        if (neg_samples[neg_samples_length] == tmp_index) {
          pos_peak_immed_follow_neg_samples[cci] = pos_sample;
          cci++;
          flag = true;
          break;
        }
      }
    }
  }

  long pos_peak_immed_follow_neg_samples_length;
  pos_peak_immed_follow_neg_samples_length = cci;

  long *cc, *cum_pd, cc_length;
  cc = pos_peak_immed_follow_neg_samples;
  cum_pd = pos_peak_immed_follow_neg_samples;
  cc_length = pos_peak_immed_follow_neg_samples_length;

  // # Per above ...
  // # "cc", and "pos_peak_immed_follow_neg_samples" & "cum_pd", are the samples (starting at 1, at the beginning of the file)
  // # at which a value in "pos_peak_samples" that is immediately preceded by a value in "neg_samples" (and not another value
  // # from "pos_peak_samples") occurs.  These points are the _possible_ approximate transition points betw "high frequency"
  // # and "low frequency" periods.

  // # pd is number of samples betw "peaks", where a "peak" is a point found in "pos_peak_immed_follow_neg_samples" (i.e., "cc").

  long freq_length, pd_length;
  freq_length = pos_peak_immed_follow_neg_samples_length-1;
  pd_length = freq_length;

  long pd[pd_length];
  double freq[freq_length];

  long all_freqs[freq_length], freq_freqs[freq_length];
  int freq_count, most_freq_index;
  freq_count = 0;
  most_freq_index = 0;

  for (j = 1; j < pos_peak_immed_follow_neg_samples_length; j++) {
    pd[j-1] = (long)(pos_peak_immed_follow_neg_samples[j] - pos_peak_immed_follow_neg_samples[j-1] + 1);

    int ii, interesting_freq_index;
    for (interesting_freq_index = freq_count, ii = 0; ii < freq_count; ii++)
      if (all_freqs[ii] == pd[j-1]) interesting_freq_index = ii;

    if (interesting_freq_index == freq_count) {
      all_freqs[interesting_freq_index] = pd[j-1];
      freq_freqs[interesting_freq_index] = 0L;
      freq_count++;
    }
    freq_freqs[interesting_freq_index]++;

    if (freq_freqs[interesting_freq_index] > freq_freqs[most_freq_index]) most_freq_index = interesting_freq_index;

    freq[j-1] = (double)sample_rate/(double)pd[j-1];
  }

  long nom_hfreq, hfreqs_lo, hfreqs_hi;
  nom_hfreq = freq_freqs[most_freq_index]; //hf_max->first;
  hfreqs_lo = (0.9 * (double)nom_hfreq);
  hfreqs_hi = ceil(1.1 * (double)nom_hfreq);

  // # Let's look for the prelude ... start with looking for at least 1000 consecutive samples containing only high frequency RF
  // # in_hf_s is a string of 1/0 values with value i indicating if the RF is in high or low frequency at sample pd[i]

  char in_hf_s[pd_length];
  for (j = 0; j < pd_length; j++) {
    in_hf_s[j] = (((pd[j]>=hfreqs_lo) && (pd[j]<=hfreqs_hi)) ? '1' : '0');
  }
  in_hf_s[pd_length+1] = (char)0;

  long num_consec_hf = ceil(1000.0/(double)hfreqs_hi);
  char target_hf[num_consec_hf];
  for (j = 0; j < num_consec_hf; j++)
    target_hf[j] = '1';

  // # Find the beginning and end of the high freq prelude (i.e., invalid data), and the high freq portion of the header (valid data)
  char *tmp_char;
  unsigned long cc_index_first_peak_of_hf_prelude;
  tmp_char = strstr(in_hf_s, target_hf);
  if (!tmp_char) exit(2);
  cc_index_first_peak_of_hf_prelude = tmp_char - in_hf_s;

  unsigned long cc_index_first_peak_of_lf_prelude;
  const char *c_tmp_char_1; c_tmp_char_1 = &(tmp_char[1]);
  tmp_char = strstr(c_tmp_char_1, "0");
  if (!tmp_char) exit(3);
  cc_index_first_peak_of_lf_prelude = tmp_char - in_hf_s;

  unsigned long cc_index_first_peak_of_hf_header;
  const char *c_tmp_char_2; c_tmp_char_2 = &(tmp_char[1]);
  tmp_char = strstr(c_tmp_char_2, "1");
  if (!tmp_char) exit(4);
  cc_index_first_peak_of_hf_header = tmp_char - in_hf_s;

  unsigned long cc_index_last_peak_of_hf_header;
  const char *c_tmp_char_3; c_tmp_char_3 = &(tmp_char[1]);
  tmp_char = strstr(c_tmp_char_3, "0");
  if (!tmp_char) exit(5);
  cc_index_last_peak_of_hf_header = tmp_char - in_hf_s;


  // # From the prelude and the header, determine a starting guess for the number of samples per bit and
  // # the locations of the start and midpoint of subsequent bits

  // # We know the size of the packet from previous experimentation
  long apx_samples_per_bit, apx_mid_bit_boundary, apx_samples_per_qrtr_bit; //, bits_per_pkt;
  apx_samples_per_bit = (cc[cc_index_last_peak_of_hf_header] - cc[cc_index_first_peak_of_hf_header]);
  apx_mid_bit_boundary = cc[cc_index_first_peak_of_hf_header];
  apx_samples_per_qrtr_bit = (long)((double)apx_samples_per_bit/4.0);
  bits_per_pkt = 69L;

  long orig_samples_per_bit;
  orig_samples_per_bit = apx_samples_per_bit;

  long first_half_sample[bits_per_pkt], second_half_sample[bits_per_pkt],
    pkt_starts[bits_per_pkt], mid_bits[bits_per_pkt];
  char first_bit_half[bits_per_pkt], second_bit_half[bits_per_pkt];
  bool valid_bit[bits_per_pkt];

  // # We will iterate on our estimate for samples per bit, increasing that estimate until
  // # we identify a validly encoded differential Manchester bit stream
  bool success;
  success = false;

  while ((!success) && ((double)apx_samples_per_bit/(double)orig_samples_per_bit < 1.2)) {
    success = true;
    for (j = 0; j < bits_per_pkt; j++) {
      if (j == 0) {
        mid_bits[j] = apx_mid_bit_boundary;
      }
      else {
        mid_bits[j] = mid_bits[j-1] + apx_samples_per_bit;
        long tmp_mid_bits;
        tmp_mid_bits = -1.0;
        for (k = 0; k < cc_length; k++) {
          if (abs(cc[k]-mid_bits[j]) > 5L) continue;
          mid_bits[j] = cc[k];
          break;
        }
      }

      pkt_starts[j] = (long)(j==0 ? (mid_bits[0] - (double)apx_samples_per_bit/2.0) : (double)(mid_bits[j] + mid_bits[j-1])/2.0);
      first_half_sample[j] = mid_bits[j] - apx_samples_per_qrtr_bit;
      second_half_sample[j] = mid_bits[j] + apx_samples_per_qrtr_bit;

      first_bit_half[j] = in_hf_s[compute_ccindex_contain_sample(first_half_sample[j], cc, cc_length)-1];
      second_bit_half[j] = in_hf_s[compute_ccindex_contain_sample(second_half_sample[j], cc, cc_length)-1];

      valid_bit[j] = first_bit_half[j] ^ second_bit_half[j];
      success &= valid_bit[j];
      if (!success) break;
      bit_vals[j] = (j==0 ? '1' : (first_bit_half[j] == second_bit_half[j-1] ? '1' : '0'));

    }
    if (success) {
      return; //  (bits = bit_vals);
    }
    else {
      apx_samples_per_bit++;
    }
  }
  return; // (bits = bit_vals);
}



