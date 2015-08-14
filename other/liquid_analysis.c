// $Id$
// $(c)$

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "liquid_analysis.h"
#include "universal_defines.h"

#define SYMBOLS_PER_MSG BITS_PER_FREESCALE_MSG
#define SAMPLES_PER_BIT 40.0f
#define UINT_SAMPLES_PER_BIT (unsigned int)SAMPLES_PER_BIT

#define CLOCK_SLOT 0

#define HIGHFREQ 1
#define LOWFREQ (-1 * HIGHFREQ)
#define HIGHBIT 1
#define LOWBIT (1-HIGHBIT)

static char app_dir[200];
static uint32_t sample_rate = 1;

static iirfilt_crcf  filter;
static msresamp_crcf resamp;
// static symsync_crcf  syncresamp;
static nco_crcf 	 nco;
static firfilt_rrrf  mf;
static freqdem       fmdemod;
// static agc_crcf      agc;

// #ifdef CHECK_FREQUENCY
// static iirfilt_rrrf  loopfilter;
// static nco_crcf      signal_nco;
// #endif

static int curr_state;

static float high_freq;
static bool high_freq_set = false;
static float low_freq;
static bool low_freq_set = false;
// static float phase = UNK;

static char basebit_vals[SYMBOLS_PER_MSG * 2 + 2];
static bool valid_pkt;
static char symbols[SYMBOLS_PER_MSG];

static uint32_t total_samples_in = 0;
static uint32_t total_samples_out = 0;

// #ifdef CHECK_FREQUENCY
// static windowf phase_errors;
// static const int max_phase_errors = 8;
// static int num_phase_errors = 0;
// #endif

static unsigned int prelude_hf_start;
static unsigned int prelude_lf_start;

int get_success() {
	return (valid_pkt ? 1 : 0);
}

char *get_app_dir() {
	return app_dir;
}

void save_sample_rate(uint32_t samp_rate) {
	sample_rate = samp_rate;
}

void reset_vars() {
	valid_pkt = false;
	curr_state = -99;

	int j;
	for (j = 0; j < SYMBOLS_PER_MSG; j++) {
		symbols[j] = '\0';
		basebit_vals[j] = '\0';
	}

	high_freq_set = false;
	low_freq_set = false;

	prelude_lf_start = UNK;
	prelude_hf_start = UNK;
	strcpy(basebit_vals, "");

// #ifdef CHECK_FREQUENCY
// 	nco_crcf_reset(signal_nco);
// 	windowf_clear(phase_errors);
// 	num_phase_errors = 0;
// #endif
}

void init_analysis() {
	float Fs              = SAMPLE_RATE * 0.001; // sample rate [kHz]
	float fsk_symbol_rate = 10.0f * (1.0f - 0.0089f) / Fs; // apx 0.004839355..
	float fsk_freq_offset = (HIGHFREQ) * 102.40 / Fs; // 0.05

// #ifdef CHECK_FREQUENCY
// 	float phase_offset      = 0.8f;     // initial phase offset
// 	float frequency_offset  = 0.01f;    // initial frequency offset
// 	float wn                = 0.10f;    // pll bandwidth
// 	float zeta              = 0.707f;   // pll damping factor
// 	float K                 = 1000.0f;  // pll loop gain

// 	float b[3];     // feedforward coefficients
// 	float a[3];     // feedback coefficients
// 	iirdes_pll_active_lag(wn, zeta, K, b, a);

// 	// create the loop filter object for pll using above coeffs
// 	loopfilter = iirfilt_rrrf_create(b, 3, a, 3);
// 	signal_nco = nco_crcf_create(LIQUID_VCO);
// #endif

	// objects
	// inf impulse respose filter w feedfwd coeff = 7 & feedback coeff = 0.05
	filter  = iirfilt_crcf_create_lowpass(7, 0.05);
	// numerically controlled oscillator that implements a "voltage-controlled" oscillator
	// that uses the sinf and cosf standard math functions to generate a complex sinusoid
	nco     = nco_crcf_create(LIQUID_VCO);
	// a multi-stage arbitrary resampler used for efficient interpolation and decimation,
	// with a resampling rate SAMPLES_PER_BIT * fsk_symbol_rate and
	// a target stop-band suppression of 60.0 dB
	// note: 5.166 samples in per 1 sample out
	resamp  = msresamp_crcf_create(UINT_SAMPLES_PER_BIT * fsk_symbol_rate, 60.0f);
	// a freqmodem object with a modulation index 1.0
	fmdemod = freqdem_create(1.0f);
	// create finite impulse response filter using Kaiser-Bessel windowed sinc method
	// w filter length = 4 * SAMPLES_PER_BIT + 1 = 81,
	// cutoff frequency = 2.0f / (float)SAMPLES_PER_BIT = 0.1,
	// stop-band attenuation = 60.0 [dB], and
	// fractional sample offset = 0.0
	mf      = firfilt_rrrf_create_kaiser(4 * UINT_SAMPLES_PER_BIT + 1,
	                                     2.0f / (float)SAMPLES_PER_BIT, 60.0f, 0.0f);

	//	 properties
	nco_crcf_set_frequency(nco, fsk_freq_offset);
	firfilt_rrrf_set_scale(mf, 0.5f);

// #ifdef CHECK_FREQUENCY
// 	phase_errors = windowf_create(8);
// 	num_phase_errors = 0;
// #endif
	total_samples_in = 0;
	total_samples_out = 0;
}

void end_analysis() {
	nco_crcf_destroy(nco);
	firfilt_rrrf_destroy(mf);
	freqdem_destroy(fmdemod);
	msresamp_crcf_destroy(resamp);
	iirfilt_crcf_destroy(filter);

// #ifdef CHECK_FREQUENCY
// 	nco_crcf_destroy(signal_nco);
// 	iirfilt_rrrf_destroy(loopfilter);
// 	windowf_destroy(phase_errors);
// #endif
}

int analyze_input(unsigned char *buf, uint32_t len, uint32_t sample_offset) {

	if (sample_offset == 0)
		init_analysis();


	int r = analyze_iq_pairs(buf, len);



	return (r);

}

int analyze_file(char *src_filename) {
	init_analysis();


	FILE *file;
	if ((file = fopen(src_filename, "rb")) == NULL) {
		LOGE("file %s could not be opened for reading.\n", src_filename);
		exit(2);
	}

	size_t bufsize = (size_t) (16 * 1024);
	unsigned char buf[(int) bufsize];
	int r = 0;

	while (!(feof(file)) && (r == 0)) {
		uint32_t n_read;
		n_read = fread(buf, sizeof(unsigned char), bufsize, file);
		r = analyze_iq_pairs(buf, n_read);
	}



	fclose(file);

	return (r);
}


int analyze_iq_pairs(unsigned char *buf, uint32_t buf_length) {
	uint32_t          b;
	uint8_t           xi, xq;

	uint8_t           *buf2;
	float complex     x;
	float complex     bias =  0.0011370 + 0.0043801 * _Complex_I;
	// float             prev_phase_error, phase_error;
	// float             freq_est;

	float complex buf_resamp[16];
	float         buf_demod[16];
	unsigned int  num_resamp;
	unsigned int  bit_val;
	unsigned int  i;
	int           ret_val;

	unsigned int p = 42;     		// starting sample
	// unsigned int q = 2713000+50000;     // ending sample

	// While there are frames in the input buffer, read and process them
	for (b = 0; (b < (unsigned long) (buf_length / 2)); b++) {
		total_samples_in++;
		if (total_samples_in < p) continue;
		// if (total_samples_in > q) continue;

		buf2 = &(buf[b * 2]);

		xi = buf2[0]; xq = buf2[1];
		// convert to float complex type
		x = ((float)(xi) - 127.0f) + ((float)(xq) - 127.0f) * _Complex_I;

		// scale and remove bias
		x = x / 128.0f - bias;

		// apply pre-processing
		iirfilt_crcf_execute(filter, x, &x);

		// remove carrier offset
		// nco_crcf_mix_down(nco, x, &x);
		// nco_crcf_step(nco);

		// resample
		msresamp_crcf_execute(resamp, &x, 1, buf_resamp, &num_resamp);

		// apply frequency discriminator
		freqdem_demodulate_block(fmdemod, buf_resamp, num_resamp, buf_demod);

		// apply 'matched' filter
		firfilt_rrrf_execute_block(mf, buf_demod, num_resamp, buf_demod);

		// save resulting sample(s)
		for (i = 0; i < num_resamp; i++) {
			if ((total_samples_out % UINT_SAMPLES_PER_BIT) == CLOCK_SLOT) {
				bit_val = (((buf_demod[i] * (float)HIGHFREQ) > 0.0f) ? HIGHBIT : LOWBIT);
				// printf("%u", bit_val > 0.0f ? 1 : 0);
				printf("%d", bit_val);
//				LOGI("%d", bit_val);

				ret_val = update_state(x, bit_val, b);
				if (ret_val == 1) {
					LOGI("(%s:%d) FINAL basebit_vals = %s\n", __FILE__, __LINE__, basebit_vals);
					LOGI("(%s:%d) FINAL symbols = %s\n", __FILE__, __LINE__, symbols);
					return ret_val;
				}
			}
			total_samples_out++;
		}

#ifdef CHECK_FREQUENCY
		nco_crcf_step(signal_nco);
#endif
	}

	return 0;
}


// #ifdef CHECK_FREQUENCY

// void check_pll_lock(nco_crcf pll_nco, int curr_state, float complex x) {
// 	if (curr_state < 0)
// 		lock_pll(pll_nco, x);
// 	else
// 		is_pll_locked();

// 	return;
// }

// bool is_pll_locked() {

// 	float avg_thresh = 0.033;
// 	float *recent_phase_errors;
// 	recent_phase_errors = malloc(8 * sizeof(float));
// 	windowf_read(phase_errors, &recent_phase_errors);

// 	float phase_error = 0.0;
// 	int k = 0;
// 	for (k = 0; k < 8; k++) {
// 		phase_error += recent_phase_errors[k];
// 	}
// 	float p = (phase_error / (float)num_phase_errors);
// 	return ((p < avg_thresh) ? true : false);

// }



// bool lock_pll(nco_crcf pll_nco, float complex x) {

// 	float complex     xt, yt;
// 	static float      phase_hat = 0.0;
// 	static float      prev_phase_hat = 0.0;
// 	static float      phase_error = 0.0;
// 	static float      prev_phase_error = 0.0;

// 	xt = cexpf(_Complex_I * x);
// 	yt = cexpf(_Complex_I * phase_hat);
// 	phase_error = cargf(xt * conjf(yt));

// 	windowf_push(phase_errors, phase_error);
// 	num_phase_errors++;
// 	if (num_phase_errors >= 2 * max_phase_errors) num_phase_errors -= max_phase_errors;

// 	iirfilt_rrrf_execute(loopfilter, phase_error, &phase_hat);

// 	int is_locked = is_pll_locked();

// 	if (is_locked == false) {
// 		nco_crcf_set_frequency(pll_nco, (phase_hat + phase_error) -
// 		                       (prev_phase_hat + prev_phase_error));
// 		nco_crcf_adjust_phase(pll_nco, phase_error);
// 	}

// 	// printf("(state: %d) %u : x = %f+j(%f), phase = %f, error = %12.8f, freq = %f rad/sample = %f Hz\n",
// 	//        bits_collected, b, crealf(x), cimagf(x), phase_hat, phase_error, *freq_var, fabs(*freq_var / (2 * M_PI) * SAMPLE_RATE));
// 	prev_phase_error = phase_error; prev_phase_hat = phase_hat;

// 	return is_locked;
// }



// void set_signal_nco_frequency(nco_crcf pll_nco, int curr_state) {
// 	if ((curr_state == 0) || (curr_state == 3)) {
// 		nco_crcf_set_frequency(pll_nco, low_freq);
// 	}
// 	else if ((curr_state == 1) || (curr_state == 2)) {
// 		nco_crcf_set_frequency(pll_nco, high_freq);
// 	}
// 	else if (curr_state % 2 == 1) {
// 		nco_crcf_set_frequency(pll_nco,
// 		                       ((basebit_vals[curr_state - 1] == '0') ? high_freq : low_freq));
// 	}
// 	return;
// }

// #endif



#define CONTINUE        return 0;
#define START_OVER      reset_vars(); CONTINUE
#define SAVE_BIT_ONLY   sprintf(basebit_vals, "%s%d", basebit_vals, b); curr_state = strlen(basebit_vals);
#define SAVE_BIT        SAVE_BIT_ONLY; CONTINUE

unsigned int update_state(complex float x, int b, unsigned int sample_num) {

  if (0 == 1) {
    x=1;
    high_freq = 0.0;
    low_freq = 0.0;
  }

// #ifdef CHECK_FREQUENCY
// 	bool pll_is_locked = is_pll_locked();
// #else
	bool pll_is_locked = true;
// #endif

	int bit_in_process = strlen(basebit_vals);

	if ((high_freq_set == false) && (low_freq_set == false)) {
		curr_state = -1;
		if (b == HIGHBIT) {
			if (prelude_hf_start == UNK)
				prelude_hf_start = sample_num;
			if ((pll_is_locked == true) &&
			        (sample_num >= prelude_hf_start + 0.0005 * SAMPLE_RATE) &&
			        (sample_num < prelude_hf_start + 0.0007 * SAMPLE_RATE)) {
// #ifdef CHECK_FREQUENCY
// 				high_freq = nco_crcf_get_frequency(signal_nco);
// #endif
				high_freq_set = true;
			}
		}
		else {
			reset_vars();
		}
	}

	if ((high_freq_set == true) && (low_freq_set == false)) {
		curr_state = -2;
		if (b == LOWBIT) {
			if (prelude_lf_start == UNK) {
				prelude_lf_start = sample_num;
			}
			if ((pll_is_locked == true) &&
			        (sample_num >= prelude_lf_start + 0.0005 * SAMPLE_RATE) &&
			        (sample_num < prelude_lf_start + 0.0007 * SAMPLE_RATE)) {
// #ifdef CHECK_FREQUENCY
// 				low_freq = nco_crcf_get_frequency(signal_nco);
// #endif
				low_freq_set = true;
				curr_state = -3;
			}
		}
		else {
			if ((pll_is_locked == true) &&
			        (sample_num >= prelude_hf_start + 0.0005 * SAMPLE_RATE) &&
			        (sample_num < prelude_hf_start + 0.0007 * SAMPLE_RATE)) {
				CONTINUE;
			}
			else {
				reset_vars();
			}
		}
	}

	if (curr_state == -3) {
		if (b == LOWBIT) {
			return 0;
		}
		else if (b == HIGHBIT) {
			if ((sample_num >= prelude_lf_start + 0.0005 * SAMPLE_RATE) &&
			        (sample_num < prelude_lf_start + 0.0007 * SAMPLE_RATE)) {
				strcat(basebit_vals, "0");
				bit_in_process++;
			}
			else {
				START_OVER
			}
		}
	}

	if ((high_freq_set == true) && (low_freq_set == true)) {
		curr_state = bit_in_process;
		if (((bit_in_process == 0) && (sample_num >= prelude_lf_start)) ||
		        (bit_in_process == 3)) {
			if (pll_is_locked == true) {
				if (b == LOWBIT) {
					SAVE_BIT
				}
				else
					START_OVER
				}
		}
		else if ((bit_in_process == 1) || (bit_in_process == 2)) {
			if (pll_is_locked == true) {
				if (b == HIGHBIT) {
					SAVE_BIT
				}
				else
					START_OVER
				}
		}
		else if (bit_in_process % 2 == 1) {
			if (pll_is_locked == true) {
				if ((bit_in_process > 136) ||
				        (b != ((int)(basebit_vals[bit_in_process - 1]) - (int)('0')))) {
					SAVE_BIT_ONLY
					if (strlen(basebit_vals) == 2 * SYMBOLS_PER_MSG) {
						int k;
						symbols[0] = '1';
						for (k = 1; k < SYMBOLS_PER_MSG - 1; k++)
							symbols[k] = ((basebit_vals[2 * k] == basebit_vals[2 * k - 1]) ? '1' : '0');
						symbols[SYMBOLS_PER_MSG - 1] = '1';
						if ((symbols[0] == '1') && (symbols[1] == '1') && (symbols[68] == '1')) {
							valid_pkt = true;
							end_analysis();
							return 1;
						}
						else
							START_OVER
						}
				}
				else { // invalid symbol
					START_OVER
				}
			}
		}
		else {
			SAVE_BIT
		}
	}

	if (strlen(basebit_vals) > 0)
		curr_state = strlen(basebit_vals);

	return 0;

}

#undef CONTINUE
#undef START_OVER
#undef SAVE_BIT_ONLY
#undef SAVE_BIT







// The following several functions are for collecting and returning specific
// pieces of collected information from a packet, and are generally called from
// code in a harness.
unsigned long get_dec_address_val( /*char *symbols, long num_bits*/ ) {
	unsigned long addr; addr = 0;
	int j; for (j = 2; j <= 29; j++) { addr = addr * 2 + ((symbols[j] == '1') ? 1 : 0); }
	LOGI("(%s:%d) addr = %ld\n", __FILE__, __LINE__, addr);
	return addr;
}

char *get_hex_address_str( /*char *symbols, long num_bits,*/ char *returned_url) {
	char *addr_str = malloc(sizeof(char) * 10);
	sprintf(addr_str, "%lX", get_dec_address_val( /*symbols, num_bits */));
	LOGI("(%s:%d) addr_str = %s\n", __FILE__, __LINE__, addr_str);
	return (returned_url = addr_str);
}

long get_temp_c( /*char *symbols, long num_bits*/ ) {
	long tempC; tempC = 0L;
	int j; for (j = 52; j <= 59; j++) { tempC = tempC * 2 + ((symbols[j] == '1') ? 1 : 0); }
	tempC -= 40;
	LOGI("(%s:%d) tempC = %ld\n", __FILE__, __LINE__, tempC);
	return tempC;
}

long get_temp_f( /*char *symbols, long num_bits*/ ) {
	long tempC; tempC = get_temp_c( /*symbols, num_bits */);
	return ((double)(tempC * 1.8)) + 32;
}

double get_pressure_kpa( /*char *symbols, long num_bits*/ ) {
	double press; press = 0.0;
	int j; for (j = 36; j <= 43; j++) { press = press * 2.0 + (symbols[j] == '1' ? 1.0 : 0.0); };
	press = (press * 2.5 - 100.0);
	LOGI("(%s:%d) press = %lf\n", __FILE__, __LINE__,  press);
	return press;
}

double get_pressure_psi( /*char *symbols, long num_bits */ ) {
	return get_pressure_kpa( /*symbols, num_bits */) * 0.145037738;
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
