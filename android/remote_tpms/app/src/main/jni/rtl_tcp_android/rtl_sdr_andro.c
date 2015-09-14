// $Id$
// $(c)$

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <fcntl.h>

#include <pthread.h>

// For exit codes
#include "RtlSdr.h"
#include "rtl_sdr_andro.h"
#include "librtlsdr_andro.h"
#include "rtl-sdr/src/convenience/convenience.h"

#define DEFAULT_SAMPLE_RATE   2048000
#define DEFAULT_BUF_LENGTH    (16 * 16384)
#define MINIMAL_BUF_LENGTH    512
#define MAXIMAL_BUF_LENGTH    (256 * 16384)

#define closesocket close
#define SOCKADDR struct sockaddr
#define SOCKET int
#define SOCKET_ERROR -1

static pthread_mutex_t ll_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t running_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct llist {
  char *data;
  size_t len;
  struct llist *next;
};

typedef struct { /* structure size must be multiple of 2 bytes */
  char magic[4];
  uint32_t tuner_type;
  uint32_t tuner_gain_count;
} dongle_info_t;

static rtlsdr_dev_t *dev = NULL;

static int global_numq = 0;
static struct llist *ll_buffers = 0;
static int llbuf_num = 500;

static volatile int do_exit = 0;
static volatile int is_running = 0;
static uint32_t bytes_to_read = 0;

void init_all_variables() {
  dev = NULL;
  global_numq = 0;
  llbuf_num = 500;
  bytes_to_read = 0;
  do_exit = 0;

  if (ll_buffers != 0) {
    struct llist *curelem, *prev;
    curelem = ll_buffers;
    ll_buffers = 0;

    while (curelem != 0) {
      prev = curelem;
      curelem = curelem->next;
      free(prev->data);
      free(prev);
    }
  }
}

static void sighandler(int signum)
{

  if (dev != NULL)
    rtlsdr_cancel_async(dev);
  if (signum != 0) {
    LOGE("Signal caught, exiting! (signal %d)", signum);
    announce_exceptioncode(
            com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_RtlSdr_EXIT_SIGNAL_CAUGHT);
  }

  do_exit = 1;
}

struct command {
  unsigned char cmd;
  unsigned int param;
} __attribute__((packed));

void rtlsdr_fc_close() {
  sighandler(0);
}

int rtlsdr_isrunning() {
  int isit_running = 0;
  pthread_mutex_lock(&running_mutex);
  isit_running = is_running;
  pthread_mutex_unlock(&running_mutex);
  return isit_running;
}

int rtlsdr_nearest_gain(rtlsdr_dev_t *dev, int target_gain) {
  int i, r, err1, err2, count, nearest;
  int* gains;
  r = rtlsdr_set_tuner_gain_mode(dev, 1);
  if (r < 0) {
    LOGE("WARNING: Failed to enable manual gain.\n");
    return r;
  }
  count = rtlsdr_get_tuner_gains(dev, NULL);
  if (count <= 0) {
    return 0;
  }
  gains = malloc(sizeof(int) * count);
  count = rtlsdr_get_tuner_gains(dev, gains);
  nearest = gains[0];
  for (i=0; i<count; i++) {
    err1 = abs(target_gain - nearest);
    err2 = abs(target_gain - gains[i]);
    if (err2 < err1) {
      nearest = gains[i];
    }
  }
  free(gains);
  return nearest;
}


void rtlsdr_main(int usbfd, const char * uspfs_path_input, int argc, char **argv)
{
  char filename[255];
  int n_read;
  int r, opt;
  int gain = 0;
  int ppm_error = 0;
  int sync_mode = 1;
  int test_mode = 0;
  FILE *file;
  uint8_t *buffer;
  int dev_index = 0;
  int dev_given = 0;
  uint32_t frequency = 100000000;
  uint32_t samp_rate = DEFAULT_SAMPLE_RATE;
  uint32_t out_block_size = DEFAULT_BUF_LENGTH;
  struct sigaction sigact;

  pthread_mutex_lock(&running_mutex);
  is_running = 1;
  pthread_mutex_unlock(&running_mutex);

  if (usbfd != -1 && (!(fcntl(usbfd, F_GETFL) != -1 || errno != EBADF))) {
    LOGE("Invalid file descriptor %d, - %s", usbfd, strerror(errno));
    announce_exceptioncode(
            com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_RtlSdr_EXIT_INVALID_FD);
    pthread_mutex_lock(&running_mutex);
    is_running = 0;
    pthread_mutex_unlock(&running_mutex);
    return;
  }

  init_all_variables();
  optind = 0;

//  LOGI("argc = %d", argc);
//  for (r = 0; r < argc; r++) {
//    LOGI("argv[%d] = %s", r, argv[r]);
//  }

  while ((opt = getopt(argc, argv, "d:f:g:s:b:n:p:t:")) != -1) {
    switch (opt) {
    case 't':
      test_mode = (strcmp(optarg, "0") == 0) ? 0 : 1;
      break;
    case 'd':
      dev_index = verbose_device_search(optarg);
      dev_given = 1;
      break;
    case 'f':
      frequency = (uint32_t)atofs(optarg);
      break;
    case 'g':
      gain = (int)(atof(optarg) * 10); /* tenths of a dB */
      break;
    case 's':
      samp_rate = (uint32_t)atofs(optarg);
      break;
    case 'p':
      ppm_error = atoi(optarg);
      break;
    case 'b':
      out_block_size = (uint32_t)atof(optarg);
      break;
    case 'n':
      bytes_to_read = (uint32_t)atof(optarg) * 2;
      LOGI("bytes_to_read = %u", bytes_to_read);
      break;
    case 'S':
      sync_mode = 1;
      break;
    default:
      LOGE("Unexpected argument '%c' with value '%s' received as an argument", opt, optarg);
//      announce_exceptioncode(com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_RtlSdr_EXIT_WRONG_ARGS);
//      pthread_mutex_lock(&running_mutex);
//      is_running = 0;
//      pthread_mutex_unlock(&running_mutex);
//      return;
      break;
    }
  }

//  LOGI("argc = %d, optind = %d, argv[%d] = %s", argc, optind, optind, argv[optind]);
  if (argc < optind) {
    LOGE("Expected at least %d arguments, but got %d", optind, argc);
    announce_exceptioncode(
            com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_RtlSdr_EXIT_WRONG_ARGS);
    pthread_mutex_lock(&running_mutex);
    is_running = 0;
    pthread_mutex_unlock(&running_mutex);
    return;
  } else {
    strcpy(filename, argv[optind]);
  }

  if (out_block_size < MINIMAL_BUF_LENGTH ||
      out_block_size > MAXIMAL_BUF_LENGTH ) {
    LOGE("Output block size wrong value, falling back to default\n");
    LOGE("Minimal length: %u\n", MINIMAL_BUF_LENGTH);
    LOGE("Maximal length: %u\n", MAXIMAL_BUF_LENGTH);
    out_block_size = DEFAULT_BUF_LENGTH;
  }

//  LOGI("bytes_to_read = %lu", bytes_to_read);

  buffer = malloc(out_block_size * sizeof(uint8_t));

  sigact.sa_handler = sighandler;
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags = 0;
  sigaction(SIGINT, &sigact, NULL);
  sigaction(SIGTERM, &sigact, NULL);
  sigaction(SIGQUIT, &sigact, NULL);
  sigaction(SIGPIPE, &sigact, NULL);

  r = 0;
  if (usbfd == -1) {
    if (!dev_given) {
      dev_index = verbose_device_search("0");
    }

    if (dev_index < 0) {
      LOGE("No supported devices found.");
      announce_exceptioncode(
              com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_RtlSdr_EXIT_NO_DEVICES);
      pthread_mutex_lock(&running_mutex);
      is_running = 0;
      pthread_mutex_unlock(&running_mutex);
      return;
    }

    r = rtlsdr_open(&dev, (uint32_t)dev_index);
  } else {
    LOGI("Opening device with fd %d at %s", usbfd, uspfs_path_input);
    r = rtlsdr_open2(&dev, dev_index, usbfd, uspfs_path_input);
    LOGI("Opened device with fd %d at %s", usbfd, uspfs_path_input);
  }

  if (r < 0) {
    LOGE("Failed to open rtlsdr device #%d.", dev_index);
    announce_exceptioncode( r );
    pthread_mutex_lock(&running_mutex);
    is_running = 0;
    pthread_mutex_unlock(&running_mutex);
    return;
  }

  if (NULL == dev) {
    LOGE("Failed to open rtlsdr device #%d.", dev_index);
    announce_exceptioncode(
            com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_RtlSdr_EXIT_FAILED_TO_OPEN_DEVICE);
    pthread_mutex_lock(&running_mutex);
    is_running = 0;
    pthread_mutex_unlock(&running_mutex);
    return;
  }

  LOGI("bytes_to_read = %u", bytes_to_read);

  rtlsdr_reset_buffer(dev);

  /* Set the sample rate */
  r = rtlsdr_set_sample_rate(dev, samp_rate);
  if (r < 0) {
    LOGE("WARNING: Failed to set sample rate.");
    announce_exceptioncode(
            com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_RtlSdr_EXIT_NOT_ENOUGH_POWER);
    pthread_mutex_lock(&running_mutex);
    is_running = 0;
    pthread_mutex_unlock(&running_mutex);
    return;
  }

  /* Set the frequency */
  r = rtlsdr_set_center_freq(dev, frequency);
  if (r < 0)
    LOGE("WARNING: Failed to set center freq.");
  else
    LOGE("Tuned to %i Hz.", frequency);

  if (0 == gain) {
    /* Enable automatic gain */
    r = rtlsdr_set_tuner_gain_mode(dev, 0);
    if (r < 0)
      LOGE("WARNING: Failed to enable automatic gain.");
    else
      LOGI("Enabled automatic gain.");
  } else {
    /* Enable manual gain */
    r = rtlsdr_set_tuner_gain_mode(dev, 1);
    if (r < 0)
      LOGE("WARNING: Failed to enable manual gain.");

    gain = rtlsdr_nearest_gain(dev, gain);

    /* Set the tuner gain */
    r = rtlsdr_set_tuner_gain(dev, gain);
    if (r < 0)
      LOGE("WARNING: Failed to set tuner gain.");
    else
      LOGE("Tuner gain set to %f dB.", gain / 10.0);
  }

  //  Set the tuner error
  verbose_ppm_set(dev, ppm_error);

  if (strcmp(filename, "-") == 0) {
    LOGE("WARNING: Failed to set output filename.");
    announce_exceptioncode(
            com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_RtlSdr_RTLSDR_FILENAME_NOT_SPECIFIED);
    pthread_mutex_lock(&running_mutex);
    is_running = 0;
    pthread_mutex_unlock(&running_mutex);
    return;
  } else {
    file = fopen(filename, "wb");
    LOGI("filename = %s\n", filename);
    if (!file) {
      LOGE("Failed to open %s\n", filename);
      announce_exceptioncode(
              com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_RtlSdr_RTLSDR_FILENAME_NOT_SPECIFIED);
      pthread_mutex_lock(&running_mutex);
      is_running = 0;
      pthread_mutex_unlock(&running_mutex);
      return;
    }
  }

//  LOGI("(%s:%d) bytes_to_read = %lu", __FILE__, __LINE__, bytes_to_read);

  /* Reset endpoint before we start reading from it (mandatory) */
  verbose_reset_buffer(dev);

//  LOGI("(%s:%d) bytes_to_read = %lu", __FILE__, __LINE__, bytes_to_read);

    LOGE("Reading samples in sync mode...\n");
    while (!do_exit) {
//      LOGI("(%s:%d) bytes_to_read = %lu", __FILE__, __LINE__, bytes_to_read);
      r = rtlsdr_read_sync(dev, buffer, out_block_size, &n_read);
      if (r < 0) {
        LOGE("WARNING: sync read failed.\n");
        break;
      }

      LOGI("(%s:%d) n_read = %d, bytes_to_read = %u", __FILE__, __LINE__, n_read, bytes_to_read);
      if ((bytes_to_read > 0) && (bytes_to_read < (uint32_t)n_read)) {
        n_read = bytes_to_read;
        do_exit = 1;
      }

      if (fwrite(buffer, 1, n_read, file) != (size_t)n_read) {
        LOGE("Short write, samples lost, exiting!\n");
        break;
      }

      if ((uint32_t)n_read < out_block_size) {
        LOGE("Short read, samples lost, exiting!\n");
        break;
      }

      if (bytes_to_read > 0)
        bytes_to_read -= n_read;
    }

  fclose(file);
  rtlsdr_close(dev);
  free(buffer);
  LOGI("c\n");

  announce_exceptioncode(com_fleetcents_remote_1tpms_fleetcentstpmsremoteclient_RtlSdr_EXIT_OK);
  LOGI("d\n");
  pthread_mutex_lock(&running_mutex);
  is_running = 0;
  pthread_mutex_unlock(&running_mutex);
  LOGI("bye!");
  return;
}
