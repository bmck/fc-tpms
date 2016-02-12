// $Id$
// $(c)$

#include <time.h>

#include "log_macros.h"
#include "state_machine.h"

static uint32_t sample_rate = 1;

static int current_state = NO_PKT;
static long samples_in_state = 0L;
static int samples_in_cycle = 0;

static int prev_event = UNK;
static unsigned char prev_b = 0x00;
static unsigned char prev_i = 0x00;
static unsigned char prev_q = 0x00;
static uint32_t prev_peak = 0;

static uint32_t prelude_startpoint = 0;
static uint32_t bit_midpoints[BITS_PER_MSG];
static uint32_t bit_startpoints[BITS_PER_MSG];
static int midpoints_found = 0;
static int startpoints_found = 0;
static bool first_half_vals[BITS_PER_MSG];
static bool second_half_vals[BITS_PER_MSG];
static int first_half_vals_found = 0;
static int second_half_vals_found = 0;
static bool bit_vals[BITS_PER_MSG];
static bool valid_pkt = false;

static char app_dir[200];

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
	// LOGI("(%s:%d) Resetting state.\n", __FILE__, __LINE__);
	int j;
	for (j = 0; j < BITS_PER_MSG; j++) {
		bit_startpoints[j] = UNK;
		bit_midpoints[j] = UNK;
		first_half_vals[j] = false;
		second_half_vals[j] = false;
		bit_vals[j] = false;
		current_state = NO_PKT;
		samples_in_state = 0;
		samples_in_cycle = 0;
	}
	first_half_vals_found = 0;
	second_half_vals_found = 0;
	startpoints_found = 0;
	midpoints_found = 0;
}

#define CURR_SAMPLE (sample_offset+i)

bool is_sample_peak(unsigned char b) {
	bool retVal; retVal = false;

	// LOGI("b = (0x%02X)\n", b);
	if (b == PEAK_VAL) {
		// LOGI("b is PEAK_VAL\n");
		retVal = (prev_event == NEG);

		// LOGI("Setting prev_even to PEAK\n");
		prev_event = PEAK;
	}
	else if (b <= MID_VAL) {
		// LOGI("b is at or below MID_VAL -- setting prev_event to NEG \n");
		prev_event = NEG;
	}

	// LOGI("Returning is_sample_peak = %s\n", retVal ? "TRUE" : "FALSE");
	return retVal;
}

int analyze_state_machine(unsigned char *buf, uint32_t len, uint32_t sample_offset) {
	uint32_t i, j, l;
	unsigned char b;

	if (sample_offset == 0) {
		reset_vars();
	}

	uint32_t byte_offset;
	byte_offset = sample_offset % 2;

	for (i = 0; i < len; i++) {
		b = buf[i];

		if (i % 2 == byte_offset) {
			bool sample_is_peak = is_sample_peak(b);

			if (samples_in_cycle > 500)
				reset_vars();

			if (sample_is_peak) {
				LOGI("(%s:%d @ t=%lu) PEAK FOUND, %d since last peak\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, samples_in_cycle);

				if (current_state == NO_PKT) {
					analyze_within_no_pkt_state(b, i, sample_offset);
				}

				// Prelude, HF period w frequency apx 1 cyc/(27/2.048e6) sec = 75.85kHz
				// Prelude lasts generally < 1150 samples = 5.6e-4 sec
				// This state will include first half of bit 0 which is always HF
				else if (current_state == HF_PRELUDE) {
					analyze_within_prelude_state(b, i, sample_offset);
				}

				// LF portions of bits 0 and 1
				else if (current_state == 0) {
					analyze_within_bit0_state(b, i, sample_offset);
				}

				// HF portion of bit 1
				else if (current_state == 1) {
					analyze_within_bit1_state(b, i, sample_offset);
				}

				// Bits 2 thru 68 (remember: there are 69 bits that are counted w 0 index)
				else if ((current_state >= 2) && (current_state < BITS_PER_MSG)) {
					analyze_within_bit_i_state(b, i, sample_offset);
				}

				// This is an unexpected problem!
				else {
					LOGI("(%s:%d @ t=%lu) How did we get to state %d?  This is a BUG.\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, current_state);
					// assert(1 == 0);
					return RTN_BUG;
				}

				prev_peak = CURR_SAMPLE;

			}
			else {
				samples_in_cycle++;
				samples_in_state++;
			}
			prev_i = b;

		}
		else {
			prev_q = b;

			struct iqpair x, y;
			x.i = prev_i; x.q = prev_q;
			// bpffilter(x, &y);
			LOGI("0x%X, 0x%X --> 0x%X, 0x%X\n", x.i, x.q, y.i, y.q);
		}

		prev_b = b;
	}

	return RTN_OK;
}

void analyze_within_no_pkt_state(unsigned char b, uint32_t i, uint32_t sample_offset) {

	if (PRECEDING_HF_CYCLE(samples_in_cycle)) {
		LOGI("(%s:%d @ t=%lu) current_state = NO_PKT, but we see a peak-to-peak w correct periodicity. Moving to HF_PRELUDE.\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE);
		current_state = HF_PRELUDE;
		prelude_startpoint = CURR_SAMPLE - samples_in_cycle;
		samples_in_state = samples_in_cycle;
		samples_in_cycle = 1;
	}
	else {
		LOGI("(%s:%d @ t=%lu) current_state = NO_PKT, but we see a valid peak.", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE);
		samples_in_cycle = 1;
	}

	return;
}

void analyze_within_prelude_state(unsigned char b, uint32_t i, uint32_t sample_offset) {
	// Prelude state runs through first half of bit 0 -- contiguous, always HF

	if (PRECEDING_HF_CYCLE(samples_in_cycle)) {
		LOGI("(%s:%d @ t=%lu) current_state = HF_PRELUDE; we see a peak (samples_in_cycle=%d), samples_in_state = %ld.\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, samples_in_cycle, samples_in_state);
		samples_in_cycle = 1;
	}
	else if (samples_in_state >= PRELUDE_SIZE) {
		LOGI("(%s:%d @ t=%lu) current_state = HF_PRELUDE; we see a peak, but w LF periodicity (ie, 2nd half of b0), samples_in_cycle = %d, and samples_in_state = %ld >= %d. We can now guess at midpt of b0. Moving to BIT0.\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, samples_in_cycle, samples_in_state, PRELUDE_SIZE);
		current_state = 0;
		first_half_vals[0] = true; first_half_vals_found++;
		bit_midpoints[0] = prev_peak; midpoints_found++;
		samples_in_cycle = samples_in_state = CURR_SAMPLE - bit_midpoints[0];
	}
	else {
		LOGI("(%s:%d @ t=%lu) current_state = HF_PRELUDE; we see a peak, but not w HF periodicity, samples_in_cycle = %d, and samples_in_state = %ld < %d. Moving back to NO_PKT.\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, samples_in_cycle, samples_in_state, PRELUDE_SIZE);
		reset_vars();
	}

	return;
}

void analyze_within_bit0_state(unsigned char b, uint32_t i, uint32_t sample_offset) {
	// Bit0 state runs from second half of bit 0 through first half of bit 1 -- contiguous, always LF

	LOGI("(%s:%d @ t=%lu) current_state = BIT0; we see a peak (samples_in_cycle=%d).\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, samples_in_cycle);
	if (PRECEDING_HF_CYCLE(samples_in_cycle)) {
		LOGI("(%s:%d @ t=%lu) current_state = BIT0; we see a HF peak (samples_in_cycle=%d), so we assume that we are at the midpoint of bit 1. Moving to BIT1.\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, samples_in_cycle);
		second_half_vals[0] = false; second_half_vals_found++;
		current_state = 1; samples_in_state = 0;
		first_half_vals[1] = false; first_half_vals_found++;
		bit_midpoints[1] = CURR_SAMPLE; midpoints_found++;
		bit_startpoints[1] = 0.5 * (bit_midpoints[0] + bit_midpoints[1]); startpoints_found++;
		bit_startpoints[0] = 2 * bit_midpoints[0] - bit_startpoints[1]; startpoints_found++;
	}
	else {
		samples_in_cycle = 1;
		samples_in_state++;
	}

	return;
}

void analyze_within_bit1_state(unsigned char b, uint32_t i, uint32_t sample_offset) {
	// Bit1 state runs through second half of bit 1 -- always HF

	if ((PRECEDING_HF_CYCLE(samples_in_cycle)) && (bit_midpoints[1] != UNK) && (bit_startpoints[1] != UNK) && (CURR_SAMPLE > bit_midpoints[1] + 1.2 * (bit_midpoints[1] - bit_startpoints[1]))) {
		LOGI("(%s:%d @ t=%lu) bit_midpoints[1]=%lu and bit_startpoints[1]=%lu.\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, (long unsigned int) bit_midpoints[1], (long unsigned int)bit_startpoints[1]);
		LOGI("(%s:%d @ t=%lu) current_state = BIT1; we are overdue for bit 2, but are still seeing HF peaks (samples_in_cycle=%d), so we assume that we are actually in bit 2. Moving to BIT2.\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, samples_in_cycle);
		current_state = 2;
		bit_startpoints[2] = 2 * bit_midpoints[1] - bit_startpoints[1]; startpoints_found++;
		samples_in_state = CURR_SAMPLE - bit_startpoints[2];
		samples_in_cycle = 1;
		analyze_within_bit_i_state(b, i, sample_offset);
	}
	else if (PRECEDING_HF_CYCLE(samples_in_cycle)) {
		second_half_vals[1] = true; second_half_vals_found++;
		samples_in_state++;
		samples_in_cycle = 1;
	}
	else {
		LOGI("(%s:%d @ t=%lu) current_state = BIT1; we see a LF peak (samples_in_cycle=%d), so we assume that we are at the beg of bit 2. Moving to BIT2.\n", __FILE__, __LINE__, (long unsigned int)CURR_SAMPLE, samples_in_cycle);
		current_state = 2;
		samples_in_state = samples_in_cycle;
		samples_in_cycle = 1;
		bit_startpoints[2] = CURR_SAMPLE - samples_in_state; startpoints_found++;
		analyze_within_bit_i_state(b, i, sample_offset);
	}

	return;
}

void analyze_within_bit_i_state(unsigned char b, uint32_t i, uint32_t sample_offset) {

	if ((bit_midpoints[current_state] == UNK) || (CURR_SAMPLE <= bit_midpoints[current_state])) {
		// we are looking at the first half of bit i
		if (sample_offset == 0)
			LOGI("(%s:%d @ t=%lu) current state is BIT%d, bit_midpoints[current_state] = %lu\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, current_state, (long unsigned int) bit_midpoints[current_state]);
		LOGI("(%s:%d @ t=%lu) a peak value (samples_in_cycle = %d)\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, samples_in_cycle);
		if (PRECEDING_HF_CYCLE(samples_in_cycle)) {
			// we have found a high freq cycle
			LOGI("(%s:%d @ t=%lu) we have finished a HF cycle (samples_in_cycle=%d), first_half_vals_found = %d \n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, samples_in_cycle, first_half_vals_found);
			if (first_half_vals_found < current_state + 1) {
				// record values
				LOGI("(%s:%d @ t=%lu) current_state = BIT%d (1H); we see a HF peak (samples_in_cycle=%d).\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, current_state, samples_in_cycle);
				first_half_vals[current_state] = true; first_half_vals_found++;
				bit_midpoints[current_state] = 2 * bit_startpoints[current_state] - bit_midpoints[current_state - 1]; midpoints_found++;
			}
		}
		else {
			// we have found a low freq cycle
			if (first_half_vals_found < current_state + 1) {
				LOGI("(%s:%d @ t=%lu) current_state = BIT%d (1H); we see a LF peak.\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, current_state);
				first_half_vals[current_state] = false; first_half_vals_found++;
				bit_midpoints[current_state] = 2 * bit_startpoints[current_state] - bit_midpoints[current_state - 1]; midpoints_found++;
			}
		}
	}
	else {
		// we are looking at the second half of bit i
		if (PRECEDING_HF_CYCLE(samples_in_cycle)) {
			// we have found a high freq cycle
			if (second_half_vals_found < current_state + 1) {
				LOGI("(%s:%d @ t=%lu) current_state = BIT%d (2H); we see a HF peak (samples_in_cycle=%d).\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, current_state, samples_in_cycle);
				if (first_half_vals[current_state] == true) {
					LOGI("(%s:%d @ t=%lu) But this yields an invalid bit. Reseting to NO_PKT.\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE);
					reset_vars();
				}
				else {
					second_half_vals[current_state] = true; second_half_vals_found++;
//                    bit_midpoints[current_state] = 2 * bit_startpoints[current_state] - bit_midpoints[current_state-1]; midpoints_found++;
				}
			}
		}
		else {
			// we have found a low freq cycle
			if (second_half_vals_found < current_state + 1) {
				LOGI("(%s:%d @ t=%lu) current_state = BIT%d (2H); we see a LF peak (samples_in_cycle=%d).\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE, current_state, samples_in_cycle);
				if (first_half_vals[current_state] == false) {
					LOGI("(%s:%d @ t=%lu) But this yields an invalid bit. Reseting to NO_PKT.\n", __FILE__, __LINE__, (long unsigned int) CURR_SAMPLE);
					reset_vars();
				}
				else {
					second_half_vals[current_state] = false; second_half_vals_found++;
//                    bit_midpoints[current_state] = 2 * bit_startpoints[current_state] - bit_midpoints[current_state-1]; midpoints_found++;
				}
			}
		}
	}

	return;
}

#undef CURR_SAMPLE

// The following several functions are for collecting and returning specific
// pieces of collected information from a packet, and are generally called from
// code in a harness.
unsigned long get_dec_address_val( /*char *bit_vals, long num_bits*/ ) {
	unsigned long addr; addr = 0;
	int j; for (j = 2; j <= 29; j++) { addr = addr * 2 + ((bit_vals[j] == true) ? 1 : 0); }
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
	int j; for (j = 52; j <= 59; j++) { tempC = tempC * 2 + ((bit_vals[j] == true) ? 1 : 0); }
	tempC -= 40;
	LOGI("(%s:%d) tempC = %ld\n", __FILE__, __LINE__, tempC);
	return tempC;
}

long get_temp_f( /*char *bit_vals, long num_bits*/ ) {
	long tempC; tempC = get_temp_c( /*bit_vals, num_bits */);
	return ((double)(tempC * 1.8)) + 32;
}

double get_pressure_kpa( /*char *bit_vals, long num_bits*/ ) {
	double press; press = 0.0;
	int j; for (j = 36; j <= 43; j++) { press = press * 2.0 + (bit_vals[j] == true ? 1.0 : 0.0); };
	press = (press * 2.5 - 100.0);
	LOGI("(%s:%d) press = %lf\n", __FILE__, __LINE__,  press);
	return press;
}

double get_pressure_psi( /*char *bit_vals, long num_bits */ ) {
	return get_pressure_kpa( /*bit_vals, num_bits */) * 0.145037738;
}

char *get_url( /*long addr, long press, long temp,*/ char *returned_url) {
	char *url = malloc(sizeof(char) * 2048);
	returned_url = url;

	time_t rawtime; time ( &rawtime );
	struct tm * ptm; ptm = gmtime ( &rawtime );

	unsigned long addr = get_dec_address_val();
	long press = get_pressure_psi();
	long tempc = get_temp_c();

	sprintf(url, "http://www.fleetcents.com/tire_samples/create?tire_sample[sensor_id]=%ld&tire_sample[receiver_id]=%d&tire_sample[psi]=%ld&tire_sample[tempc]=%ld&tire_sample[sample_time]=%d-%02d-%02d%%20%02d:%02d:%02d",
	        addr, 8, press, tempc, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	LOGI("(%s:%d) url = %s\n", __FILE__, __LINE__,  url);
	return returned_url;
}
