// $Id$
// $(c)$

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "analyze_tpms2.h"
#include "universal_defines.h"

#include "analyze_tpms_log.h"

#define SYMBOLS_PER_MSG BITS_PER_FREESCALE_MSG
#define CACHE_SIZE 65535

#define HIGHBIT 1
#define LOWBIT 0

#define MARK_HERE LOGI("%s: (%s:%d) --> here", src_name, __FILE__, __LINE__);

static FILE *file;
static char src_name[200];
static char symbols[SYMBOLS_PER_MSG + 1];
static char bits[2 * SYMBOLS_PER_MSG + 3];

unsigned long total_samples_in = 0L;
static unsigned int samples_per_sym;
static unsigned int sync_bits_start, sync_bits_end;

static bool valid_pkt;

static bool high_freq_known, low_freq_known;
static double high_freq, low_freq;
static unsigned int prelude_hf_start, prelude_lf_start;

static int preceding_bit = -1;
static float bit_confidence;
static unsigned int actual_bit_end;

static iirfilt_crcf  filter;
static nco_crcf      nco;
static bool initialized = false;

static bool read_from_cache = false;
static bool write_to_cache = true;
static int cache_loc = 0;
static int cache_count = 0;
static float complex symbol_cache[CACHE_SIZE];
static unsigned long cache_loc_in_file = 0;



char *fleet_analysis(char *fn) {
	INIT_LOGGING

	int keep = 0;
	int analyzed_ok = 1;

	LOGI("Just before file analyzed");

	analyzed_ok = analyze_file(fn);

	LOGI("Just after file analyzed");

	char* returned_string = malloc(250 * sizeof(char));
	if (analyzed_ok == 1) {

		LOGI("File analyzed ok");

		char *hexaddr = NULL; hexaddr = get_hex_address_str(hexaddr);

		LOGI("Got address");
		// const char *const_hex = hexaddr;
		// LOGI(const_hex);

		long tempc = get_temp_c();

		LOGI("Got temp and ...");

		double press_kpa = get_pressure_kpa();

		LOGI("Got pressure");

		sprintf(returned_string, "%s,%ld,%lf,", hexaddr, tempc, press_kpa);

	}

	else {
		strcpy(returned_string, ",,,");
	}

	keep = 1; //1 - analyzed_ok;
	if (keep == 0)
		unlink(fn);

	strncat(returned_string, fn, 249 - strlen(returned_string));
	return returned_string;

}







int analyze_file(char *src_filename) {
	INIT_LOGGING

	LOGI("entered analyze_file");

	if ((file = fopen(src_filename, "rb")) == NULL) {
		LOGE("%s: source file could not be opened for reading.\n", src_filename);
		return (0);
	}

	int *pkt = get_packet(src_filename);
	strcpy(src_name, src_filename);

	if (pkt) {
		// LOGI("%s: (%s:%d) FINAL basebit_vals = %s\n", src_name, __FILE__, __LINE__, basebit_vals);
		LOGI("%s: (%s:%d) FINAL symbols = %s", src_name, __FILE__, __LINE__, symbols);
		return 1;
	}
	return 0;
}

int *get_packet() { //char *src_filename) {

	int pkt_size = SYMBOLS_PER_MSG;
	float tolerance = 0.05;

	while (1) {
		write_to_cache = read_from_cache = false;
		unsigned long possible_prelude_loc = find_prelude_start();
		if (feof(file)) {
			break;
		}
		if (is_valid_prelude(possible_prelude_loc)) {
			unsigned int orig_samples_per_sym = samples_per_sym;
			unsigned int samples_per_sym_est = orig_samples_per_sym;
			int *pkt_symbols;
			// note that read_from_cache is false here, and
			// write_to_cache will be set to !read_from_cache
			// inside of try_to_get_packet_symbols

			pkt_symbols = try_to_get_packet_symbols(sync_bits_end, pkt_size, samples_per_sym_est);
			if (pkt_symbols && valid_pkt) {
				return pkt_symbols;
			}

			read_from_cache = true;
			// note that write_to_cache will be set to !read_from_cache
			// inside of try_to_get_packet_symbols
			for (samples_per_sym_est = orig_samples_per_sym - 1;
			        samples_per_sym_est > (1.0 - tolerance) * orig_samples_per_sym;
			        samples_per_sym_est--) {
				pkt_symbols = try_to_get_packet_symbols(sync_bits_end, pkt_size, samples_per_sym_est);
				if (pkt_symbols && valid_pkt) {
					return pkt_symbols;
				}
			}
			for (samples_per_sym_est = orig_samples_per_sym + 1;
			        samples_per_sym_est < (1.0 + tolerance) * orig_samples_per_sym;
			        samples_per_sym_est++) {
				pkt_symbols = try_to_get_packet_symbols(sync_bits_end, pkt_size, samples_per_sym_est);
				if (pkt_symbols && valid_pkt) {
					return pkt_symbols;
				}
			}
		}
	}

	LOGI("%s: (%s:%d) --> Failed to find packet", src_name, __FILE__, __LINE__);
	return NULL;

}

int *try_to_get_packet_symbols(unsigned int pkt_start, int num_syms, unsigned int est_samples_per_sym) {
	write_to_cache = !read_from_cache;
	cache_loc = 0;
	if (write_to_cache) {
		cache_loc_in_file = total_samples_in;
	}
	valid_pkt = true;

	LOGI("%s: (%s:%d) --> Starting to try to get packet with samples/symbol = %d", src_name, __FILE__, __LINE__, est_samples_per_sym);
	int *pkt_symbols = get_packet_symbols(pkt_start, num_syms, est_samples_per_sym);
	if (pkt_symbols && valid_pkt) {
		return pkt_symbols;
	}
	return NULL;
}

unsigned long find_prelude_start() {
	float complex *x;

	while (1) {
		x = get_next_sample();
		if ((feof(file)) || (!x)) {
			LOGI("%s: (%s:%d) --> Found end of file while looking for prelude", src_name, __FILE__, __LINE__);
			break;
		}
		float y = crealf(*x);
		free(x);
		if (y > 0.9) {
			LOGI("%s: (%s:%d) --> Found possible prelude start at location %lu", src_name, __FILE__, __LINE__, total_samples_in);
			return total_samples_in;
		}
	}
	return 0;
}

bool is_valid_prelude(unsigned long file_loc) {
	double cum_phi = 0.0;
	int count = 0;
	double phi, prev_phi, delta_phi;

	LOGI("%s: (%s:%d) ----> Starting to look for prelude at sample %lu", src_name, __FILE__, __LINE__, file_loc);

	high_freq_known = low_freq_known = false;
	prelude_hf_start = total_samples_in;
	complex float *x = get_next_sample();
	prev_phi = cargf(*x);
	free(x);

	do {
		x = get_next_sample();
		if ((!x) || (feof(file))) {
			return false;
		}
		phi = cargf(*x);
		delta_phi = fmod(fabs(2.0 * M_PI + phi - prev_phi), 2.0 * M_PI);

		cum_phi += delta_phi;
		count++;
		high_freq = cum_phi / (double) count;

		prev_phi = phi;
		free(x);

		if (count > 2000)  {
			return false;
		}

		// LOGI("%s: (%s:%d) ----> total_samples_in = %lu, %u = prelude_hf_start + 0.001 * SAMPLE_RATE", src_name, __FILE__, __LINE__, total_samples_in, prelude_hf_start + (unsigned)(0.001 * SAMPLE_RATE));
		// LOGI("%s: (%s:%d) ----> x = %f + i%f, delta_phi = %lf, cum_phi = %lf, count = %d, high_freq = %lf, fabs((delta_phi / high_freq) - 1.0) = %lf", src_name, __FILE__, __LINE__, crealf(*x), cimagf(*x), delta_phi, cum_phi, count, high_freq, fabs((delta_phi / high_freq) - 1.0));
	} while ((total_samples_in < prelude_hf_start + 0.001 * SAMPLE_RATE) ||
	         (fabs((delta_phi / high_freq) - 1.0) <= 0.05));

	high_freq_known = true;
	prelude_lf_start = total_samples_in;
	LOGI("%s: (%s:%d) ----> Found end of 1st HF phase of prelude at sample %lu, high_freq = %lf", src_name, __FILE__, __LINE__, total_samples_in, high_freq);
	cum_phi = 0.0;
	count = 0;

	do {
		x = get_next_sample();
		if ((!x) || (feof(file))) {
			return false;
		}
		phi = cargf(*x);
		delta_phi = fmod(fabs(2.0 * M_PI + phi - prev_phi), 2.0 * M_PI);
		// delta_phi = fmod(phi + M_PI * 0.5 - prev_phi, M_PI * 0.5);

		cum_phi += delta_phi;
		count++;
		low_freq = cum_phi / (double) count;

		prev_phi = phi;
		free(x);

		if (count > 2000)  {
			return false;
		}

		// LOGI("%s: (%s:%d) ----> total_samples_in = %lu, %u = prelude_lf_start + 0.001 * SAMPLE_RATE", src_name, __FILE__, __LINE__, total_samples_in, prelude_lf_start + (unsigned)(0.001 * SAMPLE_RATE));
		// LOGI("%s: (%s:%d) ----> x = %f + i%f, delta_phi = %lf, cum_phi = %lf, count = %d, low_freq = %lf, fabs((delta_phi / low_freq) - 1.0) = %lf", src_name, __FILE__, __LINE__, crealf(*x), cimagf(*x), delta_phi, cum_phi, count, low_freq, fabs((delta_phi / low_freq) - 1.0));
	} while ((total_samples_in < prelude_lf_start + 0.001 * SAMPLE_RATE) ||
	         (fabs((delta_phi / high_freq) - 1.0) > 0.10));

	low_freq_known = true;
	LOGI("%s: (%s:%d) ----> Found end of LF phase of prelude at sample %lu, low_freq = %lf", src_name, __FILE__, __LINE__, total_samples_in, low_freq);
	if (high_freq <= low_freq) {
		return false;
	}

	sync_bits_start = total_samples_in;

	do {
		x = get_next_sample();
		if ((!x) || (feof(file))) {
			return false;
		}
		phi = cargf(*x);
		delta_phi = fmod(fabs(2.0 * M_PI + phi - prev_phi), 2.0 * M_PI);
		// delta_phi = fmod(phi + M_PI * 0.5 - prev_phi, M_PI * 0.5);

		prev_phi = phi;
		free(x);

		// LOGI("%s: (%s:%d) ----> delta_phi = %lf, high_freq = %lf, fabs((delta_phi / high_freq) - 1.0) = %lf", src_name, __FILE__, __LINE__, delta_phi, high_freq, fabs((delta_phi / high_freq) - 1.0));
	} while (fabs((delta_phi / high_freq) - 1.0) <= 0.05);

	sync_bits_end = total_samples_in;
	samples_per_sym = sync_bits_end - sync_bits_start + 1;
	LOGI("%s: (%s:%d) ----> Found end of 2nd HF phase of prelude at sample %lu", src_name, __FILE__, __LINE__, total_samples_in);
	LOGI("%s: (%s:%d) ----> Estimating samples/symbol = %u", src_name, __FILE__, __LINE__, samples_per_sym);
	if (samples_per_sym < 350) {
		return false;
	}

	return true;
}

int *get_packet_symbols(unsigned int pkt_start, int num_syms, unsigned int est_samples_per_sym) {

	int *packet_syms = malloc(sizeof(int) * num_syms);
	int i = 0;
	packet_syms[i] = 1;
	strcpy(bits, "011");
	strcpy(symbols, "1");

	LOGI("%s: (%s:%d) ------> Acquired symbol %d = %d, packet still valid", src_name, __FILE__, __LINE__, i, packet_syms[i]);



	i = 1;
	packet_syms[i] = get_second_symbol(pkt_start, est_samples_per_sym);
	if (!valid_pkt) {
		// LOGI("%s: (%s:%d) ------> Acquired symbol %d = %d, packet invalid", src_name, __FILE__, __LINE__, i, packet_syms[i]);
		free(packet_syms);
		packet_syms = NULL;
		return NULL;
	}
	else {
		LOGI("%s: (%s:%d) ------> Acquired symbol %d = %d, packet still valid", src_name, __FILE__, __LINE__, i, packet_syms[i]);
		LOGI("%s: (%s:%d) ------> Bits collected = %s", src_name, __FILE__, __LINE__, bits);
		LOGI("%s: (%s:%d) ------> Symbols collected = %s", src_name, __FILE__, __LINE__, symbols);
	}



	for (i = 2; i < num_syms; i++) {
		LOGI("%s: (%s:%d) ------> Starting to acquire symbol %d at sample %u", src_name, __FILE__, __LINE__, i, actual_bit_end + 1);
		// actual_bit_end is set during the prev run of get_bit
		packet_syms[i] = get_symbol(actual_bit_end + 1, est_samples_per_sym);
		if (!valid_pkt) {
			// LOGI("%s: (%s:%d) ------> Acquired symbol %d = %d, packet invalid", src_name, __FILE__, __LINE__, i, packet_syms[i]);
			free(packet_syms);
			packet_syms = NULL;
			return NULL;
		}
		else {
			LOGI("%s: (%s:%d) ------> Acquired symbol %d = %d, packet still valid", src_name, __FILE__, __LINE__, i, packet_syms[i]);
			LOGI("%s: (%s:%d) ------> Bits collected = %s", src_name, __FILE__, __LINE__, bits);
			LOGI("%s: (%s:%d) ------> Symbols collected = %s", src_name, __FILE__, __LINE__, symbols);
		}
	}

	valid_pkt = validate_packet(packet_syms, num_syms);

	if (!valid_pkt) {
		if (packet_syms) {
			free(packet_syms);
			packet_syms = NULL;
		}
		return NULL;
	}

	return packet_syms;
}

bool validate_packet(int *packet_syms, int num_syms) {
	LOGI("%s: (%s:%d) --------> Validating packet containing %d symbols", src_name, __FILE__, __LINE__, num_syms);

	int i = 68;
	if (packet_syms[i] != 1) {
		LOGI("%s: (%s:%d) --------> Symbol %d = %d, packet invalid", src_name, __FILE__, __LINE__, i, packet_syms[i]);
		return false;
	}

	double press = 0.0;
	for (i = 0; i <= 7; i++) {
		press = press * 2.0 + packet_syms[i + 36];
		if (packet_syms[36 + i] == packet_syms[44 + i]) {
			LOGI("%s: (%s:%d) --------> Symbol %d = Symbol %d, packet invalid", src_name, __FILE__, __LINE__, 36 + i, 44 + i);
			return false;
		}
	}
	press = (press * 2.5 - 100.0);
	if (press < 0.0) {
		LOGI("%s: (%s:%d) --------> Pressure computed to be %lf < 0.0, packet invalid", src_name, __FILE__, __LINE__, press);
		return false;
	}

	return true;
}

int get_second_symbol(unsigned int sym_start, unsigned int est_samples_per_sym) {
	preceding_bit = 1;
	unsigned int samples_per_bit = est_samples_per_sym * 0.5;
	LOGI("%s: (%s:%d) --------> Starting to acquire symbol at sample %u assuming %u samples/sym", src_name, __FILE__, __LINE__, sym_start, est_samples_per_sym);
	int first_bit = 1;
	LOGI("%s: (%s:%d) --------> Starting to acquire a bit between samples %u and %u", src_name, __FILE__, __LINE__, sym_start, sym_start + samples_per_bit - 1);
	int second_bit = get_bit(sym_start, sym_start + samples_per_bit - 1);
	double second_bit_conf = bit_confidence;

	if (first_bit == second_bit) {
		if (second_bit_conf < 0.25) {
			second_bit = 1 - second_bit;
			LOGI("%s: (%s:%d) --------> Changing second bit value based on lack of confidence", src_name, __FILE__, __LINE__);
		}
		else {
			LOGI("%s: (%s:%d) --------> (First Bit) %d = (Second Bit) %d, packet invalid", src_name, __FILE__, __LINE__, first_bit, second_bit);
			valid_pkt = false;
			return 0;
		}
	}

	bits[3] = (second_bit == 0) ? '0' : '1';
	bits[4] = '\0';

	int symbol = (preceding_bit == first_bit) ? 1 : 0;
	LOGI("%s: (%s:%d) --------> Preceding Bit = %d, First Bit %d, Second Bit = %d ==>> Symbol = %d ", src_name, __FILE__, __LINE__, preceding_bit, first_bit, second_bit, symbol);
	symbols[1] = (symbol == 0) ? '0' : '1';
	symbols[2] = '\0';
	preceding_bit = second_bit;
	return symbol;
}

int get_symbol(unsigned int sym_start, unsigned int est_samples_per_sym) {
	unsigned int samples_per_bit = est_samples_per_sym / 2.0;
	LOGI("%s: (%s:%d) --------> Starting to acquire symbol at sample %u assuming %u samples/sym", src_name, __FILE__, __LINE__, sym_start, est_samples_per_sym);
	LOGI("%s: (%s:%d) --------> Starting to acquire first bit between samples %u and %u", src_name, __FILE__, __LINE__, sym_start, sym_start + samples_per_bit - 1);
	int first_bit = get_bit(sym_start, sym_start + samples_per_bit - 1);
	double first_bit_conf = bit_confidence;
	LOGI("%s: (%s:%d) --------> Starting to acquire second bit between samples %u and %u", src_name, __FILE__, __LINE__, sym_start + samples_per_bit, sym_start + samples_per_bit * 2 - 1);
	int second_bit = get_bit(actual_bit_end, sym_start + est_samples_per_sym - 1);
	double second_bit_conf = bit_confidence;

	if (first_bit == second_bit) {
		double min_conf = (first_bit_conf < second_bit_conf) ? first_bit_conf : second_bit_conf;
		if (min_conf < 0.25) {
			if (min_conf == first_bit_conf) {
				first_bit = 1 - first_bit;
				LOGI("%s: (%s:%d) --------> Changing first bit value based on lack of confidence", src_name, __FILE__, __LINE__);
			}
			else {
				second_bit = 1 - second_bit;
				LOGI("%s: (%s:%d) --------> Changing second bit value based on lack of confidence", src_name, __FILE__, __LINE__);
			}
		}
		else {
			LOGI("%s: (%s:%d) --------> (First Bit) %d = (Second Bit) %d, packet invalid", src_name, __FILE__, __LINE__, first_bit, second_bit);
			valid_pkt = false;
			return 0;
		}
	}

	int bits_collected = strlen(bits);
	bits[bits_collected] = (first_bit == 0) ? '0' : '1';
	bits[bits_collected + 1] = (second_bit == 0) ? '0' : '1';
	bits[bits_collected + 2] = '\0';

	int symbol = (preceding_bit == first_bit) ? 1 : 0;
	LOGI("%s: (%s:%d) --------> Preceding Bit = %d, First Bit %d, Second Bit = %d ==>> Symbol = %d ", src_name, __FILE__, __LINE__, preceding_bit, first_bit, second_bit, symbol);
	int symbols_collected = strlen(symbols);
	symbols[symbols_collected] = (symbol == 0) ? '0' : '1';
	symbols[symbols_collected + 1] = '\0';

	preceding_bit = second_bit;
	return symbol;
}

int get_bit(unsigned long bit_start, unsigned long bit_end) {

	unsigned long start_sample = bit_start;
	int num_samples = (bit_end - bit_start + 1);

	LOGI("%s: (%s:%d) ----------> Starting to acquire bit between samples %lu and apx %lu (possibly as far as sample %lu)", src_name, __FILE__, __LINE__, bit_start, bit_end, (long unsigned) (bit_end + num_samples * 0.1));
	complex float *samples_buffer = get_some_samples(num_samples * 1.1, bit_start);

	int transition_point = scan_samples_for_transition(samples_buffer, num_samples * 1.1);
	actual_bit_end = (transition_point == 0) ? bit_end : (transition_point - 1 + bit_start);
	if (transition_point != 0) {
		LOGI("%s: (%s:%d) ----------> Transition found at sample %u", src_name, __FILE__, __LINE__, actual_bit_end);
	}
	if (write_to_cache) {
		write_to_cache = !(read_from_cache = true);
	}
	if (read_from_cache) {
		cache_loc = cache_count - (bit_start + num_samples * 1.1 - actual_bit_end);
	}

	start_sample += 0.2 * (actual_bit_end - bit_start + 1);
	num_samples *= 0.6;
	LOGI("%s: (%s:%d) ----------> Starting to convert samples between %lu and %lu to binary (%d samples)", src_name, __FILE__, __LINE__, start_sample, start_sample + num_samples - 1, num_samples);
	int *samples = convert_samples_to_binary(samples_buffer, num_samples);
	int i, b = 0;

	for (i = 1; i < num_samples; i++) {
		b += samples[i];
	}
	bit_confidence = fabs((double) b / (double) num_samples);

	LOGI("%s: (%s:%d) ----------> num_samples = %d, b = %d", src_name, __FILE__, __LINE__, num_samples, b);

	b = (b > 0) ? HIGHBIT : LOWBIT;
	free(samples_buffer);
	free(samples);
	LOGI("%s: (%s:%d) ----------> bit returned (b) = %d", src_name, __FILE__, __LINE__, b);
	return b;
}

int *convert_samples_to_binary(complex float *samples, int num_samples) {
	int i, b = 0;
	complex float x;
	int *bit_samples = malloc(sizeof(int) * num_samples);
	double phi, prev_phi = 0.0, delta_phi;
	char tmp_samples[num_samples + 3];

	// LOGI("%s: (%s:%d) ------------> Starting to convert collected samples to binary", src_name, __FILE__, __LINE__);

	for (i = 0; i < num_samples; i++) {
		// MARK_HERE;
		x = samples[i];
		// MARK_HERE;
		phi = cargf(x);
		if (i > 0) {
			delta_phi = fmod(fabs(2.0 * M_PI + phi - prev_phi), 2.0 * M_PI);
			// delta_phi = fmod(phi + M_PI * 0.5 - prev_phi, M_PI * 0.5);
			// LOGI("%s: (%s:%d) ------------> i = %d, sample no = %lu, delta_phi = %lf, high_freq = %lf, low_freq = %lf", src_name, __FILE__, __LINE__, i, cache_loc_in_file+i, delta_phi, high_freq, low_freq);

			// if (fabs((delta_phi / high_freq) - 1.0) <= 0.05) {
			// 	b = 1;
			// }
			// else if (fabs((delta_phi / low_freq) - 1.0) <= 0.05) {
			// 	b = -1;
			// }
			// else {
			// 	b = 0;
			// }

			if (fabs((delta_phi / high_freq) - 1.0) <= fabs((delta_phi / low_freq) - 1.0)) {
				b = 1;
			}
			else {
				b = -1;
			}

			bit_samples[i] = b;
			tmp_samples[i] = (b == 1) ? '+' : (b == 0) ? '0' : '-';
		}
		else {
			bit_samples[i] = 0;
			tmp_samples[i] = ' ';
		}
		prev_phi = phi;
	}
	tmp_samples[num_samples] = '\0';
	// LOGI("%s: (%s:%d) ------------> Resulting samples are %s", src_name, __FILE__, __LINE__, tmp_samples);
	return bit_samples;

}

int scan_samples_for_transition(float complex *samples, int num_samples) {

	LOGI("%s: (%s:%d) ----------> Starting to scan samples for end-of-bit transition", src_name, __FILE__, __LINE__);

	int i, baseline_freq = 0, baseline_limit = 0.75 * num_samples;
	int *binary_samples = convert_samples_to_binary(samples, baseline_limit);
	for (i = 1; i < baseline_limit; i++) {
		baseline_freq += binary_samples[i];
	}
	double orig_freq = (baseline_freq > 0) ? high_freq : low_freq;
	double non_orig_freq = (orig_freq == high_freq) ? low_freq : high_freq;

	// LOGI("%s: (%s:%d) ----------> Baseline freq = %s = %lf", src_name, __FILE__, __LINE__, ((orig_freq == high_freq) ? "high_freq" : "low_freq"), orig_freq);

	double later_freq, phi, later_delta_phi, prev_phi = cargf(samples[baseline_limit-1]);
	for ( ; i < num_samples; i++) {
		phi = cargf(samples[i]);
		later_delta_phi = fmod(fabs(2.0 * M_PI + phi - prev_phi), 2.0 * M_PI);
		prev_phi = phi;

		later_freq = fabs((later_delta_phi/non_orig_freq) - 1.0);
		// LOGI("%s: (%s:%d) ----------> Sample freq = %lf, %lf <= %lf <= %lf ?", src_name, __FILE__, __LINE__, later_delta_phi, 0.95 * non_orig_freq, later_delta_phi, 1.05 * non_orig_freq);
		if (later_freq < 0.05) {
			LOGI("%s: (%s:%d) ----------> Transition found at bit location %d of %d", src_name, __FILE__, __LINE__, i, num_samples);
			return i;
		}
	}
	LOGI("%s: (%s:%d) ----------> Transition not found; assuming original bit end estimate", src_name, __FILE__, __LINE__);
	return 0;
}

float complex *get_some_samples(int num_samples, unsigned long start_sample_location) {
	float complex *x;

	// LOGI("%s: (%s:%d) ------------> Starting to get some samples, start_sample_location = %lu, total_samples_in = %lu, cache_loc_in_file = %lu, get_current_read_loc() = %lu, num_samples = %d",
	// 	src_name, __FILE__, __LINE__, total_samples_in, cache_loc_in_file, start_sample_location, get_current_read_loc(), num_samples);

	while (start_sample_location > get_current_read_loc()) {
		x = get_next_sample();
		if (!x) {
			return NULL;
		}
		free(x);
	}

	float complex *returned_samples = malloc(sizeof(float complex) * num_samples);

	int i;
	for (i = 0; i < num_samples; i++) {
		x = get_next_sample();
		if (!x) {
			break;
		}
		returned_samples[i] = crealf(*x) + cimagf(*x) * _Complex_I;
		free(x);
	}

	// LOGI("%s: (%s:%d) ------------> Got some samples", src_name, __FILE__, __LINE__);
	return returned_samples;
}

unsigned long get_current_read_loc() {
	unsigned long x = (read_from_cache ? (cache_loc_in_file + cache_loc) : total_samples_in);
	return x;
}

static FILE *file2;

float complex *get_next_sample() {
	if (!initialized) {
		filter  = iirfilt_crcf_create_lowpass(7, 0.05);
		nco     = nco_crcf_create(LIQUID_VCO);
		float fsk_freq_offset = -102.40e3 / SAMPLE_RATE;
		nco_crcf_set_frequency(nco, fsk_freq_offset);
		file2 = fopen("/tmp/STOU.300.cfileout", "wb");
		initialized = true;
	}

	// LOGI("%s: (%s:%d) --------------> Starting to get a sample", src_name, __FILE__, __LINE__);
	float complex *x = malloc(sizeof(float complex));

	if (read_from_cache) {
		if (cache_loc >= cache_count) {
			read_from_cache = false;
			write_to_cache = true;
		}
		else {
			(*x) = crealf(symbol_cache[cache_loc]) + cimagf(symbol_cache[cache_loc]) * _Complex_I;
			cache_loc++;
		}
	}

	if (!read_from_cache) {
		unsigned char buf[2];
		if (fread(buf, sizeof(unsigned char), 2, file) != 2) {
			return NULL;
		}

		(*x) = (((float)(buf[0]) - 127.0) + ((float)(buf[1]) - 127.0) * _Complex_I) / 128.0; // - bias;
		total_samples_in++;

		// apply pre-processing
		iirfilt_crcf_execute(filter, (*x), x);

		// remove carrier offset
		nco_crcf_mix_down(nco, (*x), x);
		nco_crcf_step(nco);

		{
			float buff[2];
			buff[0] = crealf(*x);
			buff[1] = cimagf(*x);
			fwrite(buff, sizeof(float), 2, file2);
		}

		if (write_to_cache) {
			symbol_cache[cache_loc] = crealf(*x) + cimagf(*x) * _Complex_I;
			cache_loc++;
			cache_count = cache_loc;
			if (cache_loc >= CACHE_SIZE) {
				valid_pkt = false;
				return NULL;
			}
		}

	}

	// LOGI("%s: (%s:%d) --------------> Got a sample", src_name, __FILE__, __LINE__);
	return x;
}









// The following several functions are for collecting and returning specific
// pieces of collected information from a packet, and are generally called from
// code in a harness.
int get_success() {
	return (valid_pkt ? 1 : 0);
}

unsigned long get_dec_address_val( /*char *symbols, long num_bits*/ ) {
	INIT_LOGGING
	unsigned long addr; addr = 0;
	int j; for (j = 2; j <= 29; j++) { addr = addr * 2 + ((symbols[j] == '1') ? 1 : 0); }
	LOGI("%s: (%s:%d) dec_addr = %ld", src_name, __FILE__, __LINE__, addr);
	return addr;
}

char *get_hex_address_str( /*char *symbols, long num_bits,*/ char *returned_url) {
	INIT_LOGGING
	char *addr_str = malloc(sizeof(char) * 10);
	sprintf(addr_str, "%lX", get_dec_address_val( /*symbols, num_bits */));
	LOGI("%s: (%s:%d) hex_addr = %s", src_name, __FILE__, __LINE__, addr_str);
	return (returned_url = addr_str);
}

long get_temp_c( /*char *symbols, long num_bits*/ ) {
	INIT_LOGGING
	long tempC; tempC = 0L;
	int j; for (j = 52; j <= 59; j++) { tempC = tempC * 2 + ((symbols[j] == '1') ? 1 : 0); }
	tempC -= 40;
	LOGI("%s: (%s:%d) tempC = %ld", src_name, __FILE__, __LINE__, tempC);
	return tempC;
}

long get_temp_f( /*char *symbols, long num_bits*/ ) {
	long tempC; tempC = get_temp_c( /*symbols, num_bits */);
	long tempF = ((double)(tempC * 1.8)) + 32;
	LOGI("%s: (%s:%d) tempF = %ld", src_name, __FILE__, __LINE__, tempF);
	return tempF;
}

double get_pressure_kpa( /*char *symbols, long num_bits*/ ) {
	INIT_LOGGING
	double press; press = 0.0;
	int j; for (j = 36; j <= 43; j++) { press = press * 2.0 + (symbols[j] == '1' ? 1.0 : 0.0); };
	press = (press * 2.5 - 100.0);
	LOGI("%s: (%s:%d) kpa = %lf", src_name, __FILE__, __LINE__,  press);
	return press;
}

double get_pressure_psi( /*char *symbols, long num_bits */ ) {
	double psi = get_pressure_kpa( /*symbols, num_bits */) * 0.145037738;
	LOGI("%s: (%s:%d) psi = %lf", src_name, __FILE__, __LINE__, psi);
	return psi;
}

// char *get_url( /*long addr, long press, long temp,*/ char *returned_url) {
//  INIT_LOGGING
//  char *url = malloc(sizeof(char) * 2048);
//  returned_url = url;

//  time_t rawtime; time ( &rawtime );
//  struct tm * ptm; ptm = gmtime ( &rawtime );

//  unsigned long addr = get_dec_address_val();
//  long press = get_pressure_psi();
//  long tempc = get_temp_c();
//  LOGI("%s: (%s:%d) here\n", src_name, __FILE__, __LINE__);

//  sprintf(url, "http://198.36.127.105/tire_samples/create?tire_sample[sensor_id]=%ld&tire_sample[receiver_id]=%d&tire_sample[psi]=%ld&tire_sample[tempc]=%ld&tire_sample[sample_time]=%d-%02d-%02d%%20%02d:%02d:%02d",
//          addr, 8, press, tempc, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
//  // LOGI("%s: (%s:%d) url = %s\n", src_name, __FILE__, __LINE__,  url);
//  return returned_url;
// }


