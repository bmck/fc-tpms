// $Id$
// $(c)$

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "analyze_tpms.h"
#include "universal_defines.h"

#include "vsf_log.h"

#define SYMBOLS_PER_MSG BITS_PER_FREESCALE_MSG
#define CHUNK_SIZE 256

static char app_dir[200];
static uint32_t sample_rate = 1;
static char src_name[200];

static iirfilt_crcf  filter;

static int curr_state = -99;

static float high_freq;
static bool high_freq_set = false;
static float low_freq;
static bool low_freq_set = false;

static char basebit_vals[SYMBOLS_PER_MSG * 2 + 2] = "";
static bool valid_pkt;
static char symbols[SYMBOLS_PER_MSG];
static unsigned int bit_boundary[2];
static unsigned int samples_per_bit = UNK;
static unsigned int first_bit_start = UNK;

static uint32_t total_samples_in = 0;

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

  high_freq_set = low_freq_set = false;

  prelude_lf_start = prelude_hf_start = UNK;
  strcpy(basebit_vals, "");

  bit_boundary[0] = bit_boundary[1] = UNK;
  samples_per_bit = UNK;
  first_bit_start = UNK;
}

int analyze_file(char *src_filename) {
  LOGI("entered analyze_file\n");
  filter  = iirfilt_crcf_create_lowpass(7, 0.05);
  total_samples_in = 0;

  strcpy(src_name, src_filename);

  FILE *file;
  if ((file = fopen(src_filename, "rb")) == NULL) {
    LOGE("%s: source file could not be opened for reading.\n", src_name);
    return (0);
  }

  // uint32_t          b;
  uint8_t           xi, xq;

  uint8_t           buf2[2];
  float complex     x, prev_x = 0.0;
  float complex     bias =  0.0011370 + 0.0043801 * _Complex_I;

  // float complex buf_resamp[16];
  // float         buf_demod[16];
  // unsigned int  num_resamp;
  // unsigned int  bit_val;
  // unsigned int  i;
  int           ret_val;

  total_samples_in = 0;

  // While there are frames in the input buffer, read and process them
  while (!feof(file)) {

    // size_t bytes_read =
    if (2 != fread(buf2, sizeof(uint8_t), 2, file))
      return 0;

    total_samples_in++;
    // int samples_read = bytes_read / 2;

    xi = buf2[0]; xq = buf2[1];
    // convert to float complex type
    x = ((float)(xi) - 127.0f) + ((float)(xq) - 127.0f) * _Complex_I;

    // scale and remove bias
    x = x / 128.0f - bias;

    // apply pre-processing
    iirfilt_crcf_execute(filter, x, &x);

    ret_val = update_state(x, prev_x, total_samples_in);
    if (ret_val == 1) {
      LOGI("%s: (%s:%d) FINAL basebit_vals = %s\n", src_name, __FILE__, __LINE__, basebit_vals);
      LOGI("%s: (%s:%d) FINAL symbols = %s\n", src_name, __FILE__, __LINE__, symbols);
      return ret_val;
    }

    prev_x = x;

  }

  return 0;
}






#define CONTINUE        prev_b = b; /*prev_delta_phi = delta_phi;*/ return 0;
#define START_OVER      reset_vars(); CONTINUE
#define SAVE_BIT_ONLY   LOGI("%s: (%s:%d) basebit_vals = >%s< (%d)\n", src_name, __FILE__, __LINE__, basebit_vals, (int)strlen(basebit_vals)); sprintf(basebit_vals, "%s%d", basebit_vals, b); curr_state = strlen(basebit_vals); LOGI("%s: (%s:%d) basebit_vals = >%s< (%d)\n", src_name, __FILE__, __LINE__, basebit_vals, (int)strlen(basebit_vals)); LOGI("%s: (%s:%d) Entered state %d at sample %u\n", src_name, __FILE__, __LINE__, curr_state, sample_num); prev_bit_sample = sample_num; prev_b = b;
#define SAVE_BIT        SAVE_BIT_ONLY; CONTINUE

#define HIGHFREQ(x) (fabs((x / high_freq) - 1.0) <= 0.1)
#define LOWFREQ(x)  (fabs((x / high_freq) - 1.0) >  0.1)
#define HIGHBIT 1
#define LOWBIT 0

#define CURRENT_BIT(n) (strlen(basebit_vals) == n)
#define BIT_SLOT(n) ((sample_num > first_bit_start + n * samples_per_bit) && (sample_num < first_bit_start + (n+1) * samples_per_bit))


unsigned int update_state(complex float x, complex float prev_x, unsigned int sample_num) { //, struct vsf_session* p_sess) {

  int bit_in_process = -1;
  if (curr_state != -99) {
    LOGI("%s: (%s:%d, sample %u) Current state = %d", src_name, __FILE__, __LINE__, sample_num, curr_state);
    if (curr_state > 0) {
      bit_in_process = strlen(basebit_vals);
      LOGI(", Working on bit %d (basebit_vals = %s)", bit_in_process, basebit_vals);
    }
    LOGI("\n");
  }

  int b = -1;
  float i = crealf(x);
  // float q = cimagf(x);
  double phi = cargf(x), prev_phi = cargf(prev_x);
  double delta_phi = fmod(phi + M_PI * 0.5 - prev_phi, M_PI * 0.5);
  //static double prev_delta_phi;
  static double cum_phi;
  static int count;
  static int prev_b;
  static unsigned int prev_bit_sample;

  if ((high_freq_set == false) && (low_freq_set == false)) {
    if ((i < 0.9) && (curr_state != -1)) {
      CONTINUE;
    }
    curr_state = -1;

    if (prelude_hf_start == UNK) {
      prelude_hf_start = sample_num;
      cum_phi = 0.0;
      count = 0;
    }
    else {
      cum_phi += delta_phi;
      count++;
    }

    high_freq = cum_phi / (double) count;
    LOGI("%s: (%s:%d) cum_phi = %lf, count = %d, high_freq = %lf,   ", src_name, __FILE__, __LINE__, cum_phi, count, high_freq);
    LOGI("(sample_num - prelude_hf_start) / SAMPLE_RATE = %lf,   ", (sample_num - prelude_hf_start) / SAMPLE_RATE);
    LOGI("delta_phi / high_freq = %lf\n", delta_phi / high_freq);
    if ((sample_num >= prelude_hf_start + 0.0006 * SAMPLE_RATE) &&
        (LOWFREQ(delta_phi))) {
      high_freq_set = true;
      bit_boundary[0] = sample_num;
      LOGI("%s: (%s:%d) High frequency = %lf radians per sample = %f kHz\n", src_name, __FILE__, __LINE__, high_freq, high_freq / (2000.0f * M_PI) * SAMPLE_RATE);
    }

    CONTINUE;
  }

  if ((high_freq_set == true) && (low_freq_set == false)) {
    if ((i < 0.9) && (curr_state != -2)) {
      CONTINUE;
    }
    curr_state = -2;

    if (prelude_lf_start == UNK) {
      prelude_lf_start = sample_num;
      cum_phi = 0.0;
      count = 0;
    }
    else {
      cum_phi += delta_phi;
      count++;
    }

    low_freq = cum_phi / (double) count;
    LOGI("%s: (%s:%d) cum_phi = %lf, count = %d, low_freq = %lf,   ", src_name, __FILE__, __LINE__, cum_phi, count, low_freq);
    LOGI("sample_num - prelude_lf_start / SAMPLE_RATE = %lf,   ", (sample_num - prelude_lf_start) / SAMPLE_RATE);
    LOGI("delta_phi / low_freq = %lf\n", delta_phi / low_freq);
    if ((sample_num >= prelude_lf_start + 0.0006 * SAMPLE_RATE) &&
        (HIGHFREQ(delta_phi))) {
      low_freq_set = true;
      bit_boundary[0] = sample_num;
      LOGI("%s: (%s:%d) Low frequency = %lf radians per sample = %f kHz,   ", src_name, __FILE__, __LINE__, low_freq, low_freq / (2000.0f * M_PI) * SAMPLE_RATE);
      curr_state = -3;
      LOGI("%s: (%s:%d) Entering state -3 at sample %u\n", src_name, __FILE__, __LINE__, sample_num);
    }

    CONTINUE;
  }

  if (curr_state == -3) {
    if (LOWFREQ(delta_phi)) {
      strcpy(basebit_vals, "011");
      bit_in_process += 3;
      bit_boundary[1] = sample_num;
      samples_per_bit = (bit_boundary[1] - bit_boundary[0]) * 0.5 - 1;
      first_bit_start = bit_boundary[0] - samples_per_bit;
      LOGI("%s: (%s:%d) first_bit_start = %u,   ", src_name, __FILE__, __LINE__, first_bit_start);
      LOGI("samples_per_bit = %u,   ", samples_per_bit);
      LOGI("basebit_vals = %s\n", basebit_vals);
    }
  }
  else {
    if (HIGHFREQ(delta_phi)) {
      b = HIGHBIT;
      LOGI("%s: (%s:%d) High frequency period found at sample %u\n", src_name, __FILE__, __LINE__, sample_num);
    }
    else if (LOWFREQ(delta_phi)) {
      b = LOWBIT;
      LOGI("%s: (%s:%d) Low frequency period found at sample %u\n", src_name, __FILE__, __LINE__, sample_num);
    }

    if ((high_freq_set == true) && (low_freq_set == true)) {
      LOGI("%s: (%s:%d) high_freq_set and low_freq_set are true.\n", src_name, __FILE__, __LINE__);
      curr_state = bit_in_process;
      LOGI("%s: (%s:%d) n = curr_state = bit_in_process = %d.\n", src_name, __FILE__, __LINE__, bit_in_process);
      LOGI("%s: (%s:%d) sample_num = %u, first_bit_start + n * samples_per_bit = %u, first_bit_start + n * samples_per_bit = %u\n",
           src_name, __FILE__, __LINE__, sample_num, first_bit_start + (curr_state) * samples_per_bit, first_bit_start + (curr_state + 1) * samples_per_bit);
      LOGI("%s: (%s:%d) sample_num = %u, first_bit_start + n * (samples_per_bit+1) = %u, first_bit_start + (n+1) * (samples_per_bit-1) = %u\n",
           src_name, __FILE__, __LINE__, sample_num, first_bit_start + curr_state * (samples_per_bit + 1), first_bit_start + (curr_state + 1) * (samples_per_bit - 1));

      if (CURRENT_BIT(3)) {
        LOGI("%s: (%s:%d) Bit in process = %u, b = %d\n", src_name, __FILE__, __LINE__, bit_in_process, b);
        if (b == LOWBIT) {
          LOGI("%s: (%s:%d) About to save bit\n", src_name, __FILE__, __LINE__);
          SAVE_BIT
        }
        else {
          LOGI("%s: (%s:%d) About to start over\n", src_name, __FILE__, __LINE__);
          START_OVER
        }
      }
      else if (!((b == prev_b) && (sample_num < prev_bit_sample + samples_per_bit + 5))) {
        if (bit_in_process % 2 == 1) {
          LOGI("%s: (%s:%d) Bit in process = %u\n", src_name, __FILE__, __LINE__, bit_in_process);
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
                iirfilt_crcf_destroy(filter);
                return 1;
              }
              else {
                START_OVER
              }
            }
          }
          else { // invalid symbol
            START_OVER
          }
        }
        else {
          SAVE_BIT
        }
      }
    }
  }

  if (strlen(basebit_vals) > 0)
    curr_state = strlen(basebit_vals);

  CONTINUE;

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
  LOGI("%s: (%s:%d) addr = %ld\n", src_name, __FILE__, __LINE__, addr);
  return addr;
}

char *get_hex_address_str( /*char *symbols, long num_bits,*/ char *returned_url) {
  char *addr_str = malloc(sizeof(char) * 10);
  sprintf(addr_str, "%lX", get_dec_address_val( /*symbols, num_bits */));
  LOGI("%s: (%s:%d) addr_str = %s\n", src_name, __FILE__, __LINE__, addr_str);
  return (returned_url = addr_str);
}

long get_temp_c( /*char *symbols, long num_bits*/ ) {
  long tempC; tempC = 0L;
  int j; for (j = 52; j <= 59; j++) { tempC = tempC * 2 + ((symbols[j] == '1') ? 1 : 0); }
  tempC -= 40;
  LOGI("%s: (%s:%d) tempC = %ld\n", src_name, __FILE__, __LINE__, tempC);
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
  LOGI("%s: (%s:%d) press = %lf\n", src_name, __FILE__, __LINE__,  press);
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
  LOGI("%s: (%s:%d) here\n", src_name, __FILE__, __LINE__);

  sprintf(url, "http://198.36.127.105/tire_samples/create?tire_sample[sensor_id]=%ld&tire_sample[receiver_id]=%d&tire_sample[psi]=%ld&tire_sample[tempc]=%ld&tire_sample[sample_time]=%d-%02d-%02d%%20%02d:%02d:%02d",
          addr, 8, press, tempc, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
  // LOGI("%s: (%s:%d) url = %s\n", src_name, __FILE__, __LINE__,  url);
  return returned_url;
}
