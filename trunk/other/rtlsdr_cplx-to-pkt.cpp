// IQ file converter for Software Defined Radio Programs rtl_sdr, gqrx
// from gqrx/gnuradio .cfile playback format -- complex64
// to binary packet as string
//
// compile on Linux or Mac with:  g++ ./rtlsdr_cplx-to-pkt.cpp -o rtlsdr_cplx-to-pkt
//
//

#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <complex>
#include <map>
#include <algorithm>
#include <string>

long compute_ccindex_contain_sample(long samp, long *cc, long cc_length) {
  // std::cout << "samp=" << samp;
  long cum_samp = 0L, i;
  for (i = 1; i < cc_length ; i++) {
    if ((cc[i]-cc[0]) > samp) {
      // std::cout << "    compute_ccindex_contain_sample retval=" << i << std::endl;
      return i;
    }
  }
  // std::cout << "    compute_ccindex_contain_sample retval=" << i << std::endl;
  return i;
}


int main(int argc, char *argv[])
{
  if (argc<2){
    std::cout << "usage:  rtlsdr_iq-to-pkt infile" << std::endl;
    exit(1);
  }

  const char *infilename = argv[1];
  unsigned long infilesize;
  FILE *infile = fopen(infilename, "rb");
  if (infile) {
    fseek(infile, 0L, SEEK_END);
    infilesize = (unsigned long) ftell(infile);
  }
  else {
    std::cout << "Error opening files" << std::endl;
    exit(1);
  }

  float v1, v2;
  long max_f = ceil((double)infilesize/8.0), j = 0L, k;
  double x[max_f], i[max_f], t[max_f];
  long pos_peak_samples[max_f], neg_samples[max_f];
  long ai = 0L, bi = 0L;

  // std::cout << "infilesize = " << infilesize << std::endl;
  // std::cout << "max_f = " << max_f << std::endl;
  rewind(infile);

  while (!feof(infile) && j < max_f) {
    fread((void*)(&v1), sizeof(v1), 1, infile);
    t[j] = (double)j/2.048e6;
    x[j] = (double)v1;
    // std::cout << "x[" << j << "]=" << x[j];
    // std::cout << x[j] << std::endl;
    fread((void*)(&v2), sizeof(v2), 1, infile);
    i[j] = v2;
    // std::cout << "    i[" << j << "]=" << i[j] << std::endl;
    if (x[j] > 0.995) {
      pos_peak_samples[ai] = j; ai++;
    }
    else if (x[j] < 0.0) {
      neg_samples[bi] = j; bi++;
    }
    j++;
  }
  // std::cout << std::endl << std::endl << std::endl;

  // std::cout << "x i t pos_peak_samples and neg_samples populated." << std::endl;
  fclose(infile);

  long pos_peak_samples_length = ai, neg_samples_length = bi;
  // std::cout << "pos_peak_samples_length = " << pos_peak_samples_length << "   neg_samples_length = " << neg_samples_length << std::endl;
  // long pos_peak_samples[pos_peak_samples_length], neg_samples[neg_samples_length];
  // # "pos_peak_samples" is the set of samples within the file at which the signal amplitude is approximately 1.0
  // # "neg_samples" is the set of samples within the file at which the signal amplitude is less than 0.0


  // # pos_peak_immed_follow_neg_samples is an array of transition points betw high freq and low freq.
  // # This array does not show whether it is a transition hi->lo or lo->hi, but that can be determined by
  // # sampling sufficiently far away from the transition.
  // # This transition point is not exactly accurate; the positive error will always be on the side with the high freq,
  // # and the error with average to be 50% of the high freq period.
  long pos_peak_immed_follow_neg_samples[pos_peak_samples_length], cci = 0L;
  long pos_sample = pos_peak_samples[0], prev_pos_sample;

  // std::cout << "pos_peak_samples_length = " << pos_peak_samples_length << std::endl;
  // std::cout << "pos_peak_samples = "; for (j = 0; j < pos_peak_samples_length; j++) { std::cout << " " << pos_peak_samples[j]; }; std::cout << std::endl << std::endl << std::endl;
  // std::cout << "neg_samples_length = " << neg_samples_length << std::endl;
  // std::cout << "neg_samples = "; for (j = 0; j < neg_samples_length; j++) { std::cout << " " << neg_samples[j]; }; std::cout << std::endl << std::endl << std::endl;

  for (long cindex = 1L; cindex < pos_peak_samples_length; cindex++) {
    prev_pos_sample = pos_sample;
    pos_sample = pos_peak_samples[cindex];
    for (long tmp_index = prev_pos_sample + 1; tmp_index < pos_sample; tmp_index++) {
      if (std::find(neg_samples, neg_samples+neg_samples_length, tmp_index) != neg_samples+neg_samples_length) {
        pos_peak_immed_follow_neg_samples[cci] = pos_sample;
        cci++;
        break;
      }
    }
  }

  long pos_peak_immed_follow_neg_samples_length = cci;
  // std::cout << "pos_peak_immed_follow_neg_samples_length = " << pos_peak_immed_follow_neg_samples_length << std::endl;
  // std::cout << "pos_peak_immed_follow_neg_samples = "; for (j = 0; j < pos_peak_immed_follow_neg_samples_length; j++) { std::cout << " " << pos_peak_immed_follow_neg_samples[j]; }; std::cout << std::endl << std::endl << std::endl;
  // std::cout << "pos_peak_immed_follow_neg_samples populated." << std::endl;

  long *cc = pos_peak_immed_follow_neg_samples, *cum_pd = pos_peak_immed_follow_neg_samples;
  long cc_length = pos_peak_immed_follow_neg_samples_length;
  // std::cout << "cc_length = " << cc_length << std::endl;
  // std::cout << "cc = "; for (j = 0; j < cc_length; j++) { std::cout << " " << cc[j]; }; std::cout << std::endl << std::endl << std::endl;

  // # Per above ...
  // # "cc", and "pos_peak_immed_follow_neg_samples" & "cum_pd", are the samples (starting at 1, at the beginning of the file)
  // # at which a value in "pos_peak_samples" that is immediately preceded by a value in "neg_samples" (and not another value
  // # from "pos_peak_samples") occurs.  These points are the _possible_ approximate transition points betw "high frequency"
  // # and "low frequency" periods.

  // # pd is number of samples betw "peaks", where a "peak" is a point found in "pos_peak_immed_follow_neg_samples" (i.e., "cc").

  long freq_length = pos_peak_immed_follow_neg_samples_length-1;
  long pd_length = freq_length;
  long pd[pd_length];
  double freq[freq_length];
  std::map<long, long> hfreqs;
  std::map<long, long>::iterator hf, hf_max;

  // std::cout << "pd_length = " << pd_length << std::endl;
  for (j = 1; j < pos_peak_immed_follow_neg_samples_length; j++) {
    pd[j-1] = (long)(pos_peak_immed_follow_neg_samples[j] - pos_peak_immed_follow_neg_samples[j-1] + 1);
    if ((hf = hfreqs.find(pd[j-1])) == hfreqs.end()) {
      hfreqs[pd[j-1]] = 1;
      hf = hfreqs.find(pd[j-1]);
    }
    else {
      (hf->second)++;
    }
    if ((hfreqs.size() == 1) || (hf->second > hf_max->second)) {
      hf_max = hf;
    }
    freq[j-1] = (double)2.048e6/(double)pd[j-1];
  }

  // std::cout << "pd hfreqs and freqs populated."  << std::endl;
  // std::cout << "hf_max = " << hf_max->first << ' ' << hf_max->second << std::endl;
  // std::cout << "pd = "; for (j = 0; j < pd_length; j++) { std::cout << ' ' << pd[j]; }; std::cout << std::endl;

  long nom_hfreq = hf_max->first;
  long hfreqs_lo = (0.9 * (double)nom_hfreq), hfreqs_hi = ceil(1.1 * (double)nom_hfreq);
  // std::cout << "hfreqs_lo = " << hfreqs_lo << ", hfreqs_hi = " << hfreqs_hi << std::endl;

  // # Let's look for the prelude ... start with looking for at least 1000 consecutive samples containing only high frequency RF
  // # in_hf_s is a string of 1/0 values with value i indicating if the RF is in high or low frequency at sample pd[i]

  char tmp_in_hf_s[pd_length];
  for (j = 0; j < pd_length; j++) {
    tmp_in_hf_s[j] = (((pd[j]>=hfreqs_lo) && (pd[j]<=hfreqs_hi)) ? '1' : '0');
  }
  tmp_in_hf_s[pd_length+1] = (char)0;
  // std::cout << tmp_in_hf_s << std::endl;
  std::string in_hf_s(tmp_in_hf_s);
  std::cout << "in_hf_s = " << in_hf_s << std::endl;

  long num_consec_hf = ceil(1000.0/(double)hfreqs_hi);
  std::string target_hf(num_consec_hf, '1');
  std::cout << "target_hf = " << target_hf << std::endl;

  // # Find the beginning and end of the high freq prelude (i.e., invalid data), and the high freq portion of the header (valid data)
  unsigned long cc_index_first_peak_of_hf_prelude = in_hf_s.find(target_hf);
  if (cc_index_first_peak_of_hf_prelude == in_hf_s.npos) exit(2);
  std::cout << "cc_index_first_peak_of_hf_prelude = " << cc_index_first_peak_of_hf_prelude << std::endl;

  unsigned long cc_index_first_peak_of_lf_prelude = in_hf_s.find('0',cc_index_first_peak_of_hf_prelude+1);
  if (cc_index_first_peak_of_lf_prelude == in_hf_s.npos) exit(3);
  std::cout << "cc_index_first_peak_of_lf_prelude = " << cc_index_first_peak_of_lf_prelude << std::endl;

  unsigned long cc_index_first_peak_of_hf_header = in_hf_s.find('1',cc_index_first_peak_of_lf_prelude+1);
  if (cc_index_first_peak_of_hf_header == in_hf_s.npos) exit(4);
  std::cout << "cc_index_first_peak_of_hf_header = " << cc_index_first_peak_of_hf_header << std::endl;

  unsigned long cc_index_last_peak_of_hf_header = in_hf_s.find('0', cc_index_first_peak_of_hf_header+1);
  if (cc_index_last_peak_of_hf_header == in_hf_s.npos) exit(5);
  std::cout << "cc_index_last_peak_of_hf_header = " << cc_index_last_peak_of_hf_header << std::endl;


  // # From the prelude and the header, determine a starting guess for the number of samples per bit and
  // # the locations of the start and midpoint of subsequent bits

  // # We know the size of the packet from previous experimentation
  long apx_samples_per_bit = (cc[cc_index_last_peak_of_hf_header] - cc[cc_index_first_peak_of_hf_header]),
    apx_mid_bit_boundary = cc[cc_index_first_peak_of_hf_header],
    apx_samples_per_qrtr_bit = (long)((double)apx_samples_per_bit/4.0),
    bits_per_pkt = 69L;
  long orig_samples_per_bit = apx_samples_per_bit;
  long first_half_sample[bits_per_pkt], second_half_sample[bits_per_pkt],
    pkt_starts[bits_per_pkt], mid_bits[bits_per_pkt];
  char first_bit_half[bits_per_pkt], second_bit_half[bits_per_pkt];
  bool valid_bit[bits_per_pkt];
  char bit_val[bits_per_pkt];

  std::cout << "bits_per_pkt = " << bits_per_pkt << std::endl;
  std::cout << "orig_samples_per_bit = " << orig_samples_per_bit << std::endl;


  // # We will iterate on our estimate for samples per bit, increasing that estimate until
  // # we identify a validly encoded differential Manchester bit stream
  bool success(false);

  while ((!success) && ((double)apx_samples_per_bit/(double)orig_samples_per_bit < 1.2)) {
    success = true;
    for (j = 0; j < bits_per_pkt; j++) {
      if (j == 0) {
        mid_bits[j] = apx_mid_bit_boundary;
      }
      else {
        mid_bits[j] = mid_bits[j-1] + apx_samples_per_bit;
        long tmp_mid_bits = -1.0;
        for (k = 0; k < cc_length; k++) {
          if (std::abs(cc[k]-mid_bits[j]) > 5L) continue;
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
      bit_val[j] = ((!valid_bit[j]) ? 'X' : (j==0 ? '1' : (first_bit_half[j] == second_bit_half[j-1] ? '1' : '0')));

      // std::cout << "apx_samples_per_bit=" << apx_samples_per_bit << ", j=" << j << ", pkt_start=" << pkt_starts[j] <<
      //   ", mid_bits[j]=" << mid_bits[j] << ", first_half_sample[j]=" << first_half_sample[j] <<
      //   ", second_half_sample[j]=" << second_half_sample[j] << ", first_bit_half[j]=" << first_bit_half[j] <<
      //   ", second_bit_half[j]=" << second_bit_half[j] << ", valid_bit[j]=" << valid_bit[j] << ", bit_val[j]=" << bit_val[j] <<
      //   std::endl;

    }
    if (success) {
      std::cout << std::endl << std::endl;
      std::cout << "apx_samples_per_bit = " << apx_samples_per_bit << std::endl;
      std::cout << "pkt_starts [sample instances] = "; for (j = 0; j < bits_per_pkt; j++) { std::cout << ' ' << pkt_starts[j]; } std::cout << std::endl;
      std::cout << "mid_bits [sample instances] = "; for (j = 0; j < bits_per_pkt; j++) { std::cout << ' ' << mid_bits[j]; } std::cout << std::endl;
      std::cout << "first_half_samples [sample instances] = "; for (j = 0; j < bits_per_pkt; j++) { std::cout << ' ' << first_half_sample[j]; } std::cout << std::endl;
      std::cout << "second_half_samples [sample instances] = "; for (j = 0; j < bits_per_pkt; j++) { std::cout << ' ' << second_half_sample[j]; } std::cout << std::endl;
      std::cout << "first_bit_half [baseband value] = "; for (j = 0; j < bits_per_pkt; j++) { std::cout << ' ' << first_bit_half[j]; } std::cout << std::endl;
      std::cout << "second_bit_half [baseband value] = "; for (j = 0; j < bits_per_pkt; j++) { std::cout << ' ' << second_bit_half[j]; } std::cout << std::endl;
      // std::cout << "valid_bit? = "; for (j = 0; j < bits_per_pkt; j++) { std::cout << (valid_bit[j] ? '1' : '0'); } std::cout << std::endl;
      std::cout << "bit_val [binary value] = "; for (j = 0; j < bits_per_pkt; j++) { std::cout << bit_val[j]; } std::cout << std::endl;
      std::cout << std::endl << std::endl;
      long addr = 0; for (j = 2; j <= 29; j++) { addr = addr * 2 + ((bit_val[j]=='1') ? 1 : 0); }
      std::cout << "address = " << std::hex << addr << std::dec << std::endl;
      long tempF = 0; for (j = 52; j <= 59; j++) { tempF = tempF * 2 + ((bit_val[j]=='1') ? 1 : 0); }
      std::cout << "temp = " << tempF << "C = ";
      std::cout << (tempF = ((double)(tempF-40)*1.8)+32) << "F" << std::endl;
      long press = 0; for (j = 36; j <= 43; j++) { press = press * 2 + (bit_val[j]=='1' ? 1 : 0); };
      std::cout << "pressure = " << (press = ((double)press*2.5-100.0));
      std::cout << " kPa = " << (double)press*0.145037738 << " psi " << std::endl;
    }
    else {
      apx_samples_per_bit++;
    }
  }

return 0;
}



