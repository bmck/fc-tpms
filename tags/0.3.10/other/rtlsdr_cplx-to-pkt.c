// $Id$
// $(c)$

#include "log_macros.h"
#include "fft.h"
#include "rtlsdr_cplx-to-pkt.h"

static char app_dir[200];
static int bits_per_pkt = BITS_PER_FREESCALE_MSG;
static bool success = false;
static char bit_vals[BITS_PER_FREESCALE_MSG];
static long expo = 8L;
static long block_size = IQ_SAMPLES_PER_BLOCK; //(long)(pow(2.0,expo));

// float iq_to_cplx(int b)
// {
//   return ((float)(b-127)/(float)128.0);
// }

// iqfile_to_cplxfile converts an entire file (named per infile) of IQ samples
// into a file of binary encoded complex numbers (named per outfile)
void iqfile_to_cplxfile(char *infile, char *outfile)
{
  int b;
  float f;
  FILE *i, *o;
  // LOGI("(%s:%d) infile=%s, outfile=%s\n", __FILE__, __LINE__, infile, outfile);
  i = fopen(infile, "rb"); o = fopen(outfile, "wb");
  do {
    b = fgetc(i);
    if (feof(i)) break;
    f = ((float)(b-127)/(float)128.0);
    fwrite((void*) &f, sizeof(f), 1, o);
  } while (1);
  fclose(o); fclose(i);
  return;
}

// get_complexblock_from_iqfile converts a block of 256 IQ samples from infile
// into an array of 512 interleaved floats that represent the real and
// imaginary components of corresponding complex samples
float *get_complexblock_from_iqfile(char *infile, long block_num, float *returned_f) {
  int b, j;
  FILE *i; i = fopen(infile, "rb");
  fseek(i, block_num * 256 * 2, SEEK_SET);
  for (j = 0; j < block_size * 2; j++) {
    b = fgetc(i);
    if (feof(i)) break;
    returned_f[j] = ((float)(b-127)/(float)128.0);
  }
  fclose(i);
  return returned_f;
}

// compute_ccindex_contain_sample computes the minimal value s.t.
// the sum of the array values for the positions of cc (which has length cc_length)
// from 0 to the returned value exceeds the value samp.  In other words,
// the returned value is the lowest value of j for which
// i in (0..j) sum(cc[i]) > samp
// This is useful for determining during which epoch a sample occurs.
long compute_ccindex_contain_sample(long samp, long *cc, long cc_length) {
  long i;
  for (i = 1; i < cc_length ; i++) {
    if ((cc[i]-cc[0]) > samp) {
      return i;
    }
  }
  return i;
}

// The following several functions are for collecting and returning specific
// pieces of collected information from a packet, and are generally called from
// code in a harness.
unsigned long get_dec_address_val( /*char *bit_vals, long num_bits*/ ) {
  unsigned long addr; addr = 0;
  int j; for (j = 2; j <= 29; j++) { addr = addr * 2 + ((bit_vals[j]=='1') ? 1 : 0); }
  LOGI("(%s:%d) addr = %ld\n", __FILE__, __LINE__, addr);
  return addr;
}

char *get_hex_address_str( /*char *bit_vals, long num_bits,*/ char *returned_url) {
  char *addr_str = malloc(sizeof(char) * 10);
  sprintf(addr_str, "%lX", get_dec_address_val( /*bit_vals, num_bits */));
  LOGI("(%s:%d) addr_str = %s\n", __FILE__, __LINE__, addr_str);
  return (returned_url = addr_str);
}

long get_temp_c( /*char *bit_vals, long num_bits*/ ) {
  long tempC; tempC = 0L;
  int j; for (j = 52; j <= 59; j++) { tempC = tempC * 2 + ((bit_vals[j]=='1') ? 1 : 0); }
  tempC -= 40;
  LOGI("(%s:%d) tempC = %ld\n", __FILE__, __LINE__, tempC);
  return tempC;
}

long get_temp_f( /*char *bit_vals, long num_bits*/ ) {
  long tempC; tempC = get_temp_c( /*bit_vals, num_bits */);
  return ((double)(tempC*1.8))+32;
}

double get_pressure_kpa( /*char *bit_vals, long num_bits*/ ) {
  double press; press = 0.0;
  int j; for (j = 36; j <= 43; j++) { press = press * 2.0 + (bit_vals[j]=='1' ? 1.0 : 0.0); };
  press = (press * 2.5 - 100.0);
  LOGI("(%s:%d) press = %lf\n", __FILE__, __LINE__,  press);
  return press;
}

double get_pressure_psi( /*char *bit_vals, long num_bits */ ) {
  return get_pressure_kpa( /*bit_vals, num_bits */) * 0.145037738;
}

char *get_app_dir() {
  return app_dir;
}

int get_success() {
  return (success ? 1 : 0);
}

char *get_url( /*long addr, long press, long temp,*/ char *returned_url) {
  char *url = malloc(sizeof(char) * 2048);
  returned_url = url;

  time_t rawtime; time ( &rawtime );
  struct tm * ptm; ptm = gmtime ( &rawtime );

  unsigned long addr = get_dec_address_val();
  long press = get_pressure_psi();
  long tempc = get_temp_c();

  sprintf(url, "http://server11288.baremetalcloud.com/tire_samples/create?tire_sample[sensor_id]=%ld&tire_sample[receiver_id]=%d&tire_sample[psi]=%ld&tire_sample[tempc]=%ld&tire_sample[sample_time]=%d-%02d-%02d%%20%02d:%02d:%02d",
    addr, 8, press, tempc, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  LOGI("(%s:%d) url = %s\n", __FILE__, __LINE__,  url);
  return returned_url;
}


// find_pkt_in_binfile iteratively searches a large file of binary encoded IQ samples for
// segments of high power levels, "high" being relative to the average power levels for a
// block of 256 complex samples after applying a set of Hann window coefficients.  If the
// blocks peak-to-mean power ratio is greater than x, the block is noted as being signficant,
// whereas blocks whose ratio is less than y are not significant.  Once a block is noted as
// significant, it and the immediately following n blocks are also marked as significant
// regardless of ratio values.  Consecutive blocks that are marked as significant
// are saved together in a file for further analysis.  If the further analysis yields a
// success, additional processing is foregone.

void find_pkt_in_binfile( )
{
  char binfilename[300];
  strcpy(binfilename, app_dir);
  strcat(binfilename, "/");
  strcat(binfilename, "fc.bin");

//  LOGI("(%s:%d) binfilename  = %s\n", __FILE__, __LINE__, binfilename);

  char cplxfilename[300];
  strncpy(cplxfilename, binfilename, strlen(binfilename)-3);
  cplxfilename[strlen(binfilename)-3] = (char)0;
  strcat(cplxfilename, "cfile");
//  LOGI("(%s:%d) cplxfilename  = %s\n", __FILE__, __LINE__, cplxfilename);

  success = false;
  unsigned long binfilesize;
  FILE *binfile, *cplxfile; binfile = fopen(binfilename, "rb"); cplxfile = NULL;
//  LOGI("(%s:%d) binfile  = %p\n", __FILE__, __LINE__, binfile);

  fseek(binfile, 0L, SEEK_END);
  binfilesize = (unsigned long) ftell(binfile);
  long num_blocks; num_blocks = (long)(ceil((double)binfilesize/(double)(2.0 * block_size)));
//  LOGI("(%s:%d) num_blocks = %ld\n", __FILE__, __LINE__, num_blocks);

  long curr_block, first_block;
  float returned_f[2*block_size];
  struct complex cplx_f[block_size];
  int j, file_no, fn_length; file_no = 0; fn_length = strlen(cplxfilename);
  int hysteresis_timeout, hysteresis_count; hysteresis_timeout = 3; hysteresis_count = 0;

  for (curr_block = 0L; curr_block < num_blocks; curr_block++) {
    get_complexblock_from_iqfile(binfilename, curr_block, returned_f);
    for (j = 0; j < block_size; j++) { // weighted w coeffs for Hann window
      cplx_f[j].real = returned_f[j*2] * 0.5 * (1.0 - cos(2.0*PI*j/(double)(block_size - 1)));
      cplx_f[j].imag = returned_f[j*2+1] * 0.5 * (1.0 - cos(2.0*PI*j/(double)(block_size - 1)));
    }
    fft(1, expo, cplx_f);

    double block_abs[block_size];
    double block_max, block_sum; block_max = -1.0; block_sum = 0.0;
    for (j = 0; j < block_size; j++) {
      block_abs[j] = (double) sqrt(cplx_f[j].real*cplx_f[j].real + cplx_f[j].imag*cplx_f[j].imag);
      if (block_abs[j] > block_max) block_max = block_abs[j];
      block_sum += block_abs[j];
    }
    double block_avg; block_avg = block_sum / (double)block_size;
    double block_spread; block_spread = block_max / block_avg;
//     LOGI("(%s:%d) curr_block = %ld,  block_spread = %lf\n", __FILE__, __LINE__, curr_block, block_spread);

    double def_signal_threshold, def_noise_threshold; def_signal_threshold = 8.0; def_noise_threshold = 5.0;

    if (block_spread >= def_signal_threshold) hysteresis_count = hysteresis_timeout;
    else if (block_spread < def_noise_threshold) hysteresis_count--;

    if (hysteresis_count > 0) { // keep this block
      if (cplxfile == NULL) {
         first_block = curr_block;
        sprintf(&(cplxfilename[fn_length]), "%d", file_no++);
        // LOGI("(%s:%d) OPENING cplxfile\n", __FILE__, __LINE__);
        cplxfile = fopen(cplxfilename, "wb");
        fseek( cplxfile, 0L, SEEK_SET );
      }
      // LOGI("(%s:%d) ADDING block %ld to cplxfile\n", __FILE__, __LINE__, curr_block);
      fwrite((void*) returned_f, sizeof(float), 2*block_size, cplxfile);
    }
    else { // toss this block
      if (cplxfile != NULL) {
        fclose(cplxfile);
//        LOGI("(%s:%d) CLOSING cplxfile for processing of %ld thru %ld\n", __FILE__, __LINE__, first_block, curr_block);
        get_pkt(cplxfilename);
//        remove(cplxfilename);
        cplxfile = NULL;
      }
    }
    if (success) return;
  }

//   LOGI("(%s:%d) cplxfilename  = %s\n", __FILE__, __LINE__, cplxfilename);

  // iqfile_to_cplxfile(binfilename, cplxfilename);
  // get_pkt(cplxfilename);
  }


// get_pkt is intended to be run on a file of binary encoded complex numbers that is "relatively
// certain" to contain a valid packet.  It has likely been qualified as containing a packet
// based on, e.g., an observable power level or SNR.  The packets amplitudes are interpreted
// in order to locate a prelude, header, etc.

/*char **/
void get_pkt( const char *infilename /*float sample_rate, const char *infilename, char *bits*/ )
{
//   LOGI("(%s:%d) infilename=%s\n", __FILE__, __LINE__, infilename);

  // float sample_rate = 2.048e6;

  unsigned long infilesize;
  FILE *infile = fopen(infilename, "rb");
//  LOGI("(%s:%d) infile = %p\n", __FILE__, __LINE__, infile);

  if (infile) {
    fseek(infile, 0L, SEEK_END);
    infilesize = (unsigned long) ftell(infile);
  }
  else {
    exit(1);
  }

  float v1, v2;
  long max_f, j, k;
  max_f = (long) (ceil((double)infilesize/8.0));
//  LOGI("(%s:%d) max_f=%ld\n", __FILE__, __LINE__, max_f);

  double peak_threshold = 0.995;
  long prev, ci;
  ci = 0L; j = 0L; prev = 0;

  rewind(infile);

  while (!feof(infile) && j < max_f) {
    // v1 and v2 are complex number coeffs
    fread((void*)(&v1), sizeof(v1), 1, infile);
    fread((void*)(&v2), sizeof(v2), 1, infile);

    if (((double)v1) > peak_threshold) {
        if (prev == -1) {
        ci++;
      }
      prev = 1;
    }
    else if (((double)v1) < 0.0) {
        prev = -1;
    }
    j++;
  }

  rewind(infile);

  // # pos_peak_immed_follow_neg_samples is an array of transition points betw high freq and low freq.
  // # This array does not show whether it is a transition hi->lo or lo->hi, but that can be determined by
  // # sampling sufficiently far away from the transition.
  // # This transition point is not exactly accurate; the positive error will always be on the side with the high freq,
  // # and the error with average to be 50% of the high freq period.

  long pos_peak_immed_follow_neg_samples_length;
  if ((pos_peak_immed_follow_neg_samples_length = ci) < 2) return;

//   LOGI("(%s:%d) pos_peak_immed_follow_neg_samples_length = %ld\n", __FILE__, __LINE__, pos_peak_immed_follow_neg_samples_length);

  long pos_peak_immed_follow_neg_samples[pos_peak_immed_follow_neg_samples_length];
  ci = 0; j = 0L; prev = 0;

  while (!feof(infile) && j < max_f) {
    fread((void*)(&v1), sizeof(v1), 1, infile);
    fread((void*)(&v2), sizeof(v2), 1, infile);

    if (((double)v1) > peak_threshold) {
      if (prev == -1) {
        pos_peak_immed_follow_neg_samples[ci] = j;
//         LOGI("(%s:%d) pos_peak_immed_follow_neg_samples[%ld] = %ld\n", __FILE__, __LINE__, ci, j);
        ci++;
      }
      prev = 1;
    }
    else if (((double)v1) < 0.0) {
      prev = -1;
    }
    j++;
  }

  fclose(infile);

  long *cc, cc_length;
  cc = pos_peak_immed_follow_neg_samples;
  cc_length = pos_peak_immed_follow_neg_samples_length;

  // # Per above ...
  // # "cc", and "pos_peak_immed_follow_neg_samples", are the samples
  // # (starting at 1, at the beginning of the file)
  // # that are the _possible_ approximate transition points betw "high frequency"
  // # and "low frequency" periods.

  // # pd is number of samples betw "peaks", where a "peak" is a point found in "pos_peak_immed_follow_neg_samples" (i.e., "cc").

  long freq_length, pd_length;
  freq_length = pos_peak_immed_follow_neg_samples_length-1;
  pd_length = freq_length;

  long pd[pd_length];

  long all_freqs[freq_length], freq_freqs[freq_length];
  int freq_count, most_freq_index, ii, interesting_freq_index;
  freq_count = 0;
  most_freq_index = 0;

  for (j = 1; j < pos_peak_immed_follow_neg_samples_length; j++) {
    pd[j-1] = (long)(pos_peak_immed_follow_neg_samples[j] -
      pos_peak_immed_follow_neg_samples[j-1] + 1);
    // LOGI("(%s:%d) pd[%ld] = %ld\n", __FILE__, __LINE__, j-1, pd[j-1]);

    for (interesting_freq_index = freq_count, ii = 0; ii < freq_count; ii++)
      if (all_freqs[ii] == pd[j-1]) interesting_freq_index = ii;

    if (interesting_freq_index == freq_count) {
      all_freqs[interesting_freq_index] = pd[j-1];
      freq_freqs[interesting_freq_index] = 0L;
      freq_count++;
    }
    freq_freqs[interesting_freq_index]++;

    if (freq_freqs[interesting_freq_index] > freq_freqs[most_freq_index])
      most_freq_index = interesting_freq_index;

//     LOGI("(%s:%d) (j=%ld) most_freq_index = %d, most_freq_freq = %ld\n",
//       __FILE__, __LINE__, j, most_freq_index, freq_freqs[most_freq_index]);
  }

  long nom_hfreq, hfreqs_lo, hfreqs_hi;
  nom_hfreq = all_freqs[most_freq_index];
  hfreqs_lo = (long)(0.85 * (double)nom_hfreq);
  hfreqs_hi = (long)(ceil(1.15 * (double)nom_hfreq));

  // for (j = 0; j < freq_count; j++) {
  //   LOGI("(%s:%d) %ld: freq = %ld, count of the freq = %ld \n", __FILE__, __LINE__, j, all_freqs[j], freq_freqs[j]);
  // }

  // LOGI("(%s:%d) nom_hfreq = %ld \n", __FILE__, __LINE__, nom_hfreq);
  // LOGI("(%s:%d) hfreqs = %ld ... %ld \n", __FILE__, __LINE__, hfreqs_lo, hfreqs_hi);

  // # Let's look for the prelude ... start with looking for at least 1000 consecutive samples containing only high frequency RF
  // # in_hf_s is a string of 1/0 values with value i indicating if the RF is in high or low frequency at sample pd[i]

  char in_hf_s[pd_length+1];
  strcpy(&(in_hf_s[pd_length-1]), " ");
  for (j = 0; j < pd_length; j++)
    in_hf_s[j] = (((pd[j]>=hfreqs_lo) && (pd[j]<=hfreqs_hi)) ? '1' : '0');

  // LOGI("(%s:%d) in_hf_s = %s\n", __FILE__, __LINE__, in_hf_s);

  long num_consec_hf = (long)(ceil(1000.0/(double)hfreqs_hi));
  char target_hf[num_consec_hf];
  strcpy(&(target_hf[num_consec_hf-1]), " ");
  for (j = 0; j < num_consec_hf; j++)
    target_hf[j] = '1';

  // LOGI("(%s:%d) target_hf = %s\n", __FILE__, __LINE__, target_hf);

  // # Find the beginning and end of the high freq prelude (i.e., invalid data), and the high freq portion of the header (valid data)
  char *tmp_char;
  // unsigned long cc_index_first_peak_of_hf_prelude;
  tmp_char = strstr(in_hf_s, target_hf);
  if (!tmp_char) return;
  // cc_index_first_peak_of_hf_prelude = tmp_char - in_hf_s;
  // LOGI("(%s:%d) cc_index_first_peak_of_hf_prelude = %lu\n", __FILE__, __LINE__, cc_index_first_peak_of_hf_prelude);

  // unsigned long cc_index_first_peak_of_lf_prelude;
  const char *c_tmp_char_1; c_tmp_char_1 = &(tmp_char[1]);
  tmp_char = strstr(c_tmp_char_1, "0");
  if (!tmp_char) return;
  // cc_index_first_peak_of_lf_prelude = tmp_char - in_hf_s;
  // LOGI("(%s:%d) cc_index_first_peak_of_lf_prelude = %lu\n", __FILE__, __LINE__, cc_index_first_peak_of_lf_prelude);

  unsigned long cc_index_first_peak_of_hf_header;
  const char *c_tmp_char_2; c_tmp_char_2 = &(tmp_char[1]);
  tmp_char = strstr(c_tmp_char_2, "1");
  if (!tmp_char) return;
  cc_index_first_peak_of_hf_header = tmp_char - in_hf_s;
  // LOGI("(%s:%d) cc_index_first_peak_of_hf_header = %lu\n", __FILE__, __LINE__, cc_index_first_peak_of_hf_header);

  unsigned long cc_index_last_peak_of_hf_header;
  const char *c_tmp_char_3; c_tmp_char_3 = &(tmp_char[1]);
  tmp_char = strstr(c_tmp_char_3, "0");
  if (!tmp_char) return;
  cc_index_last_peak_of_hf_header = tmp_char - in_hf_s;
  // LOGI("(%s:%d) cc_index_last_peak_of_hf_header = %lu\n", __FILE__, __LINE__, cc_index_last_peak_of_hf_header);

  // # From the prelude and the header, determine a starting guess for the number of samples per bit and
  // # the locations of the start and midpoint of subsequent bits

  // # We know the size of the packet from previous experimentation
  long apx_samples_per_bit, apx_mid_bit_boundary, apx_samples_per_qrtr_bit; //, bits_per_pkt;
  apx_samples_per_bit = (cc[cc_index_last_peak_of_hf_header] - cc[cc_index_first_peak_of_hf_header]);
  apx_mid_bit_boundary = cc[cc_index_first_peak_of_hf_header];
  apx_samples_per_qrtr_bit = (long)((double)apx_samples_per_bit/4.0);

  long orig_samples_per_bit;
  orig_samples_per_bit = apx_samples_per_bit;

  long first_half_sample[bits_per_pkt], second_half_sample[bits_per_pkt],
    pkt_starts[bits_per_pkt], mid_bits[bits_per_pkt];
  char first_bit_half[bits_per_pkt], second_bit_half[bits_per_pkt];
  bool valid_bit[bits_per_pkt];

  // LOGI("(%s:%d) here\n", __FILE__, __LINE__);

  // # We will iterate on our estimate for samples per bit, increasing that estimate until
  // # we identify a validly encoded differential Manchester bit stream
  success = false;

  while ((!success) && ((double)apx_samples_per_bit/(double)orig_samples_per_bit < 1.2)) {
    success = true;
    for (j = 0; j < bits_per_pkt; j++) {
      if (j == 0) {
        mid_bits[j] = apx_mid_bit_boundary;
      }
      else {
        mid_bits[j] = mid_bits[j-1] + apx_samples_per_bit;
        // long tmp_mid_bits;
        // tmp_mid_bits = -1.0;
        for (k = 0; k < cc_length; k++) {
          if (labs(cc[k]-mid_bits[j]) > 5L) continue;
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
      LOGI("(%s:%d) here; bit_vals = %s\n", __FILE__, __LINE__, bit_vals);

      return; //  (bits = bit_vals);
    }
    else {
      apx_samples_per_bit++;
    }
  }
  // LOGI("(%s:%d) here\n", __FILE__, __LINE__);

  return; // (bits = bit_vals);
}
