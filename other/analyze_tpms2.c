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
#define CHUNK_SIZE 32768

#define MARK_HERE LOGI("%s: (%s:%d) --> here", src_name, __FILE__, __LINE__);


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







static FILE *file;
static char src_name[200];
static char symbols[SYMBOLS_PER_MSG + 1];
static char bits[2*SYMBOLS_PER_MSG + 3];

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


unsigned long total_samples_in = 0L;

float complex *get_some_samples(int num_samples, unsigned long start_sample_location) {
	float complex *returned_samples = malloc(sizeof(float complex) * num_samples);
	float complex *x;

	while (start_sample_location < total_samples_in) {
		x = get_next_sample();
		if (!x) {
			free(returned_samples);
			return NULL;
		}
		free(x);
	}

	int i;
	for (i = 0; i < num_samples; i++) {
		x = get_next_sample();
		if (!x) {
			break;
		}
		memcpy(&(returned_samples[i]), x, sizeof(float complex));
		free(x);
	}

	return returned_samples;
}

static iirfilt_crcf  filter;
static nco_crcf      nco;
static bool initialized = false;

float complex *get_next_sample() {
	if (!initialized) {
		filter  = iirfilt_crcf_create_lowpass(7, 0.05);
		nco     = nco_crcf_create(LIQUID_VCO);
		float fsk_freq_offset = -102.40e3 / SAMPLE_RATE;
		nco_crcf_set_frequency(nco, fsk_freq_offset);
		initialized = true;
	}

	unsigned char buf[2];
	if (fread(buf, sizeof(unsigned char), 2, file) != 2) {
		return NULL;
	}

	float complex *x = malloc(sizeof(float complex));
	(*x) = (((float)(buf[0]) - 127.0) + ((float)(buf[1]) - 127.0) * _Complex_I) / 128.0; // - bias;
	total_samples_in++;

	// apply pre-processing
	iirfilt_crcf_execute(filter, (*x), x);

	// remove carrier offset
	nco_crcf_mix_down(nco, (*x), x);
	nco_crcf_step(nco);

	return x;
}



static unsigned int samples_per_sym;
static unsigned int sync_bits_start, sync_bits_end;
static bool valid_pkt;

int get_success() {
	return (valid_pkt ? 1 : 0);
}

int *get_packet() { //char *src_filename) {

	int pkt_size = SYMBOLS_PER_MSG;
	// int *pkt = malloc(sizeof(int) * pkt_size);
	float tolerance = 0.05;

	while (1) {
		unsigned long possible_prelude_loc = find_prelude_start();
		if (feof(file)) {
			break;
		}
		if (is_valid_prelude(possible_prelude_loc)) {
			unsigned int orig_samples_per_sym = samples_per_sym;
			unsigned int samples_per_sym_est;

			for (samples_per_sym_est = samples_per_sym;
			        samples_per_sym_est > (1.0 - tolerance) * orig_samples_per_sym;
			        samples_per_sym_est--) {
				valid_pkt = true;
				LOGI("%s: (%s:%d) --> Starting to try to get packet with samples/symbol = %d", src_name, __FILE__, __LINE__, samples_per_sym_est);
				int *pkt_symbols = get_packet_symbols(sync_bits_end, pkt_size, samples_per_sym_est);
				if (pkt_symbols && valid_pkt) {
					return pkt_symbols;
				}
			}
			for (samples_per_sym_est = samples_per_sym + 1;
			        samples_per_sym_est < (1.0 + tolerance) * orig_samples_per_sym;
			        samples_per_sym_est++) {
				valid_pkt = true;
				LOGI("%s: (%s:%d) --> Starting to try to get packet with samples/symbol = %d", src_name, __FILE__, __LINE__, samples_per_sym_est);
				int *pkt_symbols = get_packet_symbols(sync_bits_end, pkt_size, samples_per_sym_est);
				if (pkt_symbols && valid_pkt) {
					return pkt_symbols;
				}
			}
		}
	}

	LOGI("%s: (%s:%d) --> Failed to find packet", src_name, __FILE__, __LINE__);
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

static bool high_freq_known, low_freq_known;
static double high_freq, low_freq;
static unsigned int prelude_hf_start, prelude_lf_start;

bool is_valid_prelude(unsigned long file_loc) {
	double cum_phi = 0.0;
	int count = 0;
	double phi, prev_phi, delta_phi;
	// float y;

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
		// y = cabsf(*x);
		phi = cargf(*x);
		free(x);
		// if (y < 0.9) {
		// 	return false;
		// }
		delta_phi = fmod(phi + M_PI * 0.5 - prev_phi, M_PI * 0.5);

		cum_phi += delta_phi;
		count++;
		high_freq = cum_phi / (double) count;

		prev_phi = phi;
		// LOGI("%s: (%s:%d) ----> total_samples_in = %lu, %u = prelude_hf_start + 0.0006 * SAMPLE_RATE", src_name, __FILE__, __LINE__, total_samples_in, prelude_hf_start + (unsigned)(0.0006 * SAMPLE_RATE));
		// LOGI("%s: (%s:%d) ----> delta_phi = %lf, high_freq = %lf, fabs((delta_phi / high_freq) - 1.0) = %lf", src_name, __FILE__, __LINE__, delta_phi, high_freq, fabs((delta_phi / high_freq) - 1.0));
	} while ((total_samples_in < prelude_hf_start + 0.0006 * SAMPLE_RATE) ||
	         (fabs((delta_phi / high_freq) - 1.0) <= 0.05));

	high_freq_known = true;
	prelude_lf_start = total_samples_in;
	LOGI("%s: (%s:%d) ----> Found end of 1st HF phase of prelude at sample %lu", src_name, __FILE__, __LINE__, total_samples_in);
	cum_phi = 0.0;
	count = 0;

	do {
		x = get_next_sample();
		if ((!x) || (feof(file))) {
			return false;
		}
		// y = cabsf(*x);
		phi = cargf(*x);
		free(x);
		// if (y < 0.9) {
		// 	return false;
		// }
		delta_phi = fmod(phi + M_PI * 0.5 - prev_phi, M_PI * 0.5);

		cum_phi += delta_phi;
		count++;
		low_freq = cum_phi / (double) count;

		prev_phi = phi;
		// LOGI("%s: (%s:%d) ----> total_samples_in = %lu, %u = prelude_lf_start + 0.0006 * SAMPLE_RATE", src_name, __FILE__, __LINE__, total_samples_in, prelude_lf_start + (unsigned)(0.0006 * SAMPLE_RATE));
		// LOGI("%s: (%s:%d) ----> delta_phi = %lf, low_freq = %lf, fabs((delta_phi / low_freq) - 1.0) = %lf", src_name, __FILE__, __LINE__, delta_phi, low_freq, fabs((delta_phi / low_freq) - 1.0));
	} while ((total_samples_in < prelude_lf_start + 0.0006 * SAMPLE_RATE) ||
	         (fabs((delta_phi / high_freq) - 1.0) > 0.05));

	low_freq_known = true;
	LOGI("%s: (%s:%d) ----> Found end of LF phase of prelude at sample %lu", src_name, __FILE__, __LINE__, total_samples_in);

	sync_bits_start = total_samples_in;

	do {
		x = get_next_sample();
		if ((!x) || (feof(file))) {
			return false;
		}
		// y = cabsf(*x);
		phi = cargf(*x);
		free(x);
		// if (y < 0.9) {
		// 	return false;
		// }
		delta_phi = fmod(phi + M_PI * 0.5 - prev_phi, M_PI * 0.5);

		prev_phi = phi;
		// LOGI("%s: (%s:%d) ----> delta_phi = %lf, high_freq = %lf, fabs((delta_phi / high_freq) - 1.0) = %lf", src_name, __FILE__, __LINE__, delta_phi, high_freq, fabs((delta_phi / high_freq) - 1.0));
	} while (fabs((delta_phi / high_freq) - 1.0) <= 0.10);

	sync_bits_end = total_samples_in;
	samples_per_sym = sync_bits_end - sync_bits_start + 1;
	LOGI("%s: (%s:%d) ----> Found end of 2nd HF phase of prelude at sample %lu", src_name, __FILE__, __LINE__, total_samples_in);
	LOGI("%s: (%s:%d) ----> Estimating samples/symbol = %u", src_name, __FILE__, __LINE__, samples_per_sym);

	return true;
}

int *get_packet_symbols(unsigned int pkt_start, int num_syms, unsigned int est_samples_per_sym) {

	strcpy(bits, "011");
	strcpy(symbols, "1");

	int *packet_syms = malloc(sizeof(int) * num_syms);
	packet_syms[0] = 1; // Hardcoded bits 0 and 1
	LOGI("%s: (%s:%d) ------> Acquired symbol %d = %d, packet still valid", src_name, __FILE__, __LINE__, 0, packet_syms[0]);

	packet_syms[1] = get_second_symbol(pkt_start, est_samples_per_sym);

	int i;
	for (i = 2; i < num_syms; i++) {
		LOGI("%s: (%s:%d) ------> Starting to acquire symbol %d at sample %u", src_name, __FILE__, __LINE__, i, pkt_start + i * samples_per_sym);
		packet_syms[i] = get_symbol(pkt_start + (i-1.5) * samples_per_sym, est_samples_per_sym);
		if (!valid_pkt) {
			LOGI("%s: (%s:%d) ------> Acquired symbol %d = %d, packet invalid", src_name, __FILE__, __LINE__, i, packet_syms[i]);
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
	// validate packet symbols
	if (packet_syms[68] != 1) {
		LOGI("%s: (%s:%d) ------> Symbol %d = %d, packet invalid", src_name, __FILE__, __LINE__, 69, packet_syms[68]);
		valid_pkt = false;
	}

	double press = 0.0;
	for (i = 0; i <= 7; i++) {
		press = press * 2.0 + packet_syms[i + 36];
		if (packet_syms[36 + i] == packet_syms[44 + i]) {
			LOGI("%s: (%s:%d) ------> Symbol %d = Symbol %d, packet invalid", src_name, __FILE__, __LINE__, 36 + i, 44 + i);
			valid_pkt = false;
		}
	}
	press = (press * 2.5 - 100.0);
	if (press < 0.0) {
		LOGI("%s: (%s:%d) ------> Pressure computed to be %lf < 0.0, packet invalid", src_name, __FILE__, __LINE__, press);
		valid_pkt = false;
	}

	if (!valid_pkt) {
		if (packet_syms) {
			free(packet_syms);
			packet_syms = NULL;
		}
		return NULL;
	}

	return packet_syms;
}

static int preceding_bit = -1;

int get_second_symbol(unsigned int sym_start, unsigned int est_samples_per_sym) {

    preceding_bit = 1;
	unsigned int samples_per_bit = est_samples_per_sym * 0.5;
	LOGI("%s: (%s:%d) --------> Starting to acquire symbol at sample %u assuming %u samples/sym", src_name, __FILE__, __LINE__, sym_start, est_samples_per_sym);
	int first_bit = 1;
	LOGI("%s: (%s:%d) --------> Starting to acquire first bit between samples %u and %u", src_name, __FILE__, __LINE__, sym_start, sym_start + samples_per_bit - 1);
	int second_bit = get_bit(sym_start, sym_start + samples_per_bit - 1);
	bits[3] = (second_bit == 0) ? '0' : '1';
	bits[4] = 0x0;

	if (first_bit == second_bit) {
		LOGI("%s: (%s:%d) --------> (First Bit) %d = (Second Bit) %d, packet invalid", src_name, __FILE__, __LINE__, first_bit, second_bit);
		valid_pkt = false;
	}

	int symbol = (preceding_bit == first_bit) ? 1 : 0;
	LOGI("%s: (%s:%d) --------> Preceding Bit = %d, First Bit %d, Second Bit = %d ==>> Symbol = %d ", src_name, __FILE__, __LINE__, preceding_bit, first_bit, second_bit, symbol);
	symbols[1] = (symbol == 0) ? '0' : '1';
	symbols[2] = 0x0;
	preceding_bit = second_bit;
	return symbol;
}

int get_symbol(unsigned int sym_start, unsigned int est_samples_per_sym) {

	unsigned int samples_per_bit = est_samples_per_sym / 2.0;
	LOGI("%s: (%s:%d) --------> Starting to acquire symbol at sample %u assuming %u samples/sym", src_name, __FILE__, __LINE__, sym_start, est_samples_per_sym);
	LOGI("%s: (%s:%d) --------> Starting to acquire first bit between samples %u and %u", src_name, __FILE__, __LINE__, sym_start, sym_start + samples_per_bit - 1);
	int first_bit = get_bit(sym_start, sym_start + samples_per_bit - 1);
	LOGI("%s: (%s:%d) --------> Starting to acquire first bit between samples %u and %u", src_name, __FILE__, __LINE__, sym_start + samples_per_bit, sym_start + samples_per_bit * 2 - 1);
	int second_bit = get_bit(sym_start + samples_per_bit, sym_start + 2 * samples_per_bit - 1);

	int bits_collected = strlen(bits);
	bits[bits_collected] = (first_bit == 0) ? '0' : '1';
	bits[bits_collected+1] = (second_bit == 0) ? '0' : '1';
	bits[bits_collected+2] = 0x0;

	if (first_bit == second_bit) {
		LOGI("%s: (%s:%d) --------> (First Bit) %d = (Second Bit) %d, packet invalid", src_name, __FILE__, __LINE__, first_bit, second_bit);
		valid_pkt = false;
	}

	int symbol = (preceding_bit == first_bit) ? 1 : 0;
	LOGI("%s: (%s:%d) --------> Preceding Bit = %d, First Bit %d, Second Bit = %d ==>> Symbol = %d ", src_name, __FILE__, __LINE__, preceding_bit, first_bit, second_bit, symbol);
	int symbols_collected = strlen(symbols);
	symbols[symbols_collected] = (symbol == 0) ? '0' : '1';
	symbols[symbols_collected+1] = 0x0;

	preceding_bit = second_bit;
	return symbol;
}

#define HIGHBIT 1
#define LOWBIT 0

int get_bit(unsigned long bit_start, unsigned long bit_end) {

	int num_samples = bit_end - bit_start + 1;
	// float file_samples_buffer[num_samples * 2];

	LOGI("%s: (%s:%d) ----------> Starting to acquire bit between samples %lu and %lu", src_name, __FILE__, __LINE__, bit_start, bit_end);
	LOGI("%s: (%s:%d) ----------> Starting to get some samples and convert them to binary", src_name, __FILE__, __LINE__);
	complex float *samples_buffer = get_some_samples(num_samples, bit_start);
	int *samples = convert_samples_to_binary(samples_buffer, num_samples);
	int i, b = 0;

	for (i = 1; i < num_samples; i++) {
		b += samples[i];
	}
	LOGI("%s: (%s:%d) ----------> num_samples = %d, b = %d", src_name, __FILE__, __LINE__, num_samples, b);

	b = (b > 0) ? HIGHBIT : LOWBIT;
	free(samples_buffer);
	free(samples);
	LOGI("%s: (%s:%d) ----------> bit returned (b) = %d", src_name, __FILE__, __LINE__, b);
	return b;
}

int *convert_samples_to_binary(complex float *samples, int num_samples) {

	int i;
	complex float x;
	int b = 0;
	int *bit_samples = malloc(sizeof(int) * num_samples);
	double phi, prev_phi = 0.0, delta_phi;
	// LOGI("%s: (%s:%d) ------------> Starting to convert collected samples to binary", src_name, __FILE__, __LINE__);

	for (i = 0; i < num_samples; i++) {
		x = samples[i];
		phi = cargf(x);
		if (i > 0) {
			delta_phi = fmod(phi + M_PI * 0.5 - prev_phi, M_PI * 0.5);
			// LOGI("%s: (%s:%d) ------------> i = %d, delta_phi = %lf, high_freq = %lf, low_freq = %lf", src_name, __FILE__, __LINE__, i, delta_phi, high_freq, low_freq);

			if (fabs((delta_phi / high_freq) - 1.0) <= 0.05) {
				b = 1;
			}
			else if (fabs((delta_phi / low_freq) - 1.0) <= 0.5) {
				b = -1;
			}
			else {
				b = 0;
			}

			bit_samples[i] = b;
		}
		prev_phi = phi;
	}
	char tmp_samples[num_samples+3];
	for (i = 0; i < num_samples; i++) {
		tmp_samples[i] = (b == 1) ? '+' : (b == 0) ? '0' : '-';
	}
	tmp_samples[num_samples] = 0x0;
	// LOGI("%s: (%s:%d) ------------> Resulting samples are %s", src_name, __FILE__, __LINE__, tmp_samples);
	return bit_samples;

}







// The following several functions are for collecting and returning specific
// pieces of collected information from a packet, and are generally called from
// code in a harness.
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


