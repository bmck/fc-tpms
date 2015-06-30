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
		struct llist *curelem,*prev;
		curelem = ll_buffers;
		ll_buffers = 0;

		while(curelem != 0) {
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
		aprintf_stderr("Signal caught, exiting! (signal %d)", signum);
		announce_exceptioncode(com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_SIGNAL_CAUGHT);
	}

	do_exit = 1;
}

void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx)
{
	if(!do_exit) {
		struct llist *rpt = (struct llist*)malloc(sizeof(struct llist));
		rpt->data = (char*)malloc(len);
		memcpy(rpt->data, buf, len);
		rpt->len = len;
		rpt->next = NULL;

		pthread_mutex_lock(&ll_mutex);

		if (ll_buffers == NULL) {
			ll_buffers = rpt;
		} else {
			struct llist *cur = ll_buffers;
			int num_queued = 0;

			while (cur->next != NULL) {
				cur = cur->next;
				num_queued++;
			}

			if(llbuf_num && llbuf_num == num_queued-2){
				struct llist *curelem;

				free(ll_buffers->data);
				curelem = ll_buffers->next;
				free(ll_buffers);
				ll_buffers = curelem;
			}

			cur->next = rpt;

			global_numq = num_queued;
		}
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&ll_mutex);
	}
}

struct command{
	unsigned char cmd;
	unsigned int param;
}__attribute__((packed));

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

void rtlsdr_main(int usbfd, const char * uspfs_path_input, int argc, char **argv)
{
  char filename[255];
  int n_read;
  int r, opt;
  int gain = 0;
  int ppm_error = 0;
  int sync_mode = 0;
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
   aprintf_stderr("Invalid file descriptor %d, - %s", usbfd, strerror(errno));
   announce_exceptioncode(com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_INVALID_FD);
   pthread_mutex_lock(&running_mutex);
   is_running = 0;
   pthread_mutex_unlock(&running_mutex);
   return;
  }

  init_all_variables();

  while ((opt = getopt(argc, argv, "d:f:g:s:b:n:p:S")) != -1) {
    switch (opt) {
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
      aprintf("bytes_to_read = %lu", bytes_to_read);
      break;
    case 'S':
      sync_mode = 1;
      break;
    default:
     aprintf_stderr("Unexpected argument '%c' with value '%s' received as an argument", opt, optarg);
     announce_exceptioncode(com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_WRONG_ARGS);
     pthread_mutex_lock(&running_mutex);
     is_running = 0;
     pthread_mutex_unlock(&running_mutex);
     return;
    }
  }

  if (argc <= optind) {
   aprintf_stderr("Expected at least %d arguments, but got %d", optind, argc);
   announce_exceptioncode(com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_WRONG_ARGS);
   pthread_mutex_lock(&running_mutex);
   is_running = 0;
   pthread_mutex_unlock(&running_mutex);
   return;
  } else {
    strcpy(filename, argv[optind]);
  }

  if(out_block_size < MINIMAL_BUF_LENGTH ||
     out_block_size > MAXIMAL_BUF_LENGTH ){
    aprintf_stderr("Output block size wrong value, falling back to default\n");
    aprintf_stderr("Minimal length: %u\n", MINIMAL_BUF_LENGTH);
    aprintf_stderr("Maximal length: %u\n", MAXIMAL_BUF_LENGTH);
    out_block_size = DEFAULT_BUF_LENGTH;
  }

  aprintf("bytes_to_read = %lu", bytes_to_read);

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
       aprintf_stderr("No supported devices found.");
       announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_NO_DEVICES );
       pthread_mutex_lock(&running_mutex);
       is_running = 0;
       pthread_mutex_unlock(&running_mutex);
       return;
     }

    r = rtlsdr_open(&dev, (uint32_t)dev_index);
  } else {
    aprintf("Opening device with fd %d at %s", usbfd, uspfs_path_input);
    r = rtlsdr_open2(&dev, dev_index, usbfd, uspfs_path_input);
  }

  if (r < 0) {
    aprintf_stderr("Failed to open rtlsdr device #%d.", dev_index);
    announce_exceptioncode( r );
    pthread_mutex_lock(&running_mutex);
    is_running = 0;
    pthread_mutex_unlock(&running_mutex);
    return;
  }

  if (NULL == dev) {
    aprintf_stderr("Failed to open rtlsdr device #%d.", dev_index);
    announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_FAILED_TO_OPEN_DEVICE );
    pthread_mutex_lock(&running_mutex);
    is_running = 0;
    pthread_mutex_unlock(&running_mutex);
    return;
  }

  aprintf("bytes_to_read = %lu", bytes_to_read);

  //  Set the tuner error
  verbose_ppm_set(dev, ppm_error);

  /* Set the sample rate */
  r = rtlsdr_set_sample_rate(dev, samp_rate);
  if (r < 0) {
   aprintf_stderr("WARNING: Failed to set sample rate.");
   announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_NOT_ENOUGH_POWER );
   pthread_mutex_lock(&running_mutex);
   is_running = 0;
   pthread_mutex_unlock(&running_mutex);
   return;
  }

  /* Set the frequency */
  r = rtlsdr_set_center_freq(dev, frequency);
  if (r < 0)
   aprintf_stderr("WARNING: Failed to set center freq.");
  else
   aprintf_stderr("Tuned to %i Hz.", frequency);

  if (0 == gain) {
   /* Enable automatic gain */
   r = rtlsdr_set_tuner_gain_mode(dev, 0);
   if (r < 0)
     aprintf_stderr("WARNING: Failed to enable automatic gain.");
  } else {
   /* Enable manual gain */
   r = rtlsdr_set_tuner_gain_mode(dev, 1);
   if (r < 0)
     aprintf_stderr("WARNING: Failed to enable manual gain.");

   /* Set the tuner gain */
   r = rtlsdr_set_tuner_gain(dev, gain);
   if (r < 0)
     aprintf_stderr("WARNING: Failed to set tuner gain.");
   else
     aprintf_stderr("Tuner gain set to %f dB.", gain/10.0);
  }

  if(strcmp(filename, "-") == 0) {
   aprintf_stderr("WARNING: Failed to set output filename.");
   announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_RTLSDR_FILENAME_NOT_SPECIFIED );
   pthread_mutex_lock(&running_mutex);
   is_running = 0;
   pthread_mutex_unlock(&running_mutex);
   return;
  } else {
    file = fopen(filename, "wb");
    aprintf("filename = %s\n", filename);
    if (!file) {
      aprintf_stderr("Failed to open %s\n", filename);
      announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_RTLSDR_FILENAME_NOT_SPECIFIED );
      pthread_mutex_lock(&running_mutex);
      is_running = 0;
      pthread_mutex_unlock(&running_mutex);
      return;
    }
  }

  aprintf("(%s:%d) bytes_to_read = %lu", __FILE__, __LINE__, bytes_to_read);

  /* Reset endpoint before we start reading from it (mandatory) */
  verbose_reset_buffer(dev);

  aprintf("(%s:%d) bytes_to_read = %lu", __FILE__, __LINE__, bytes_to_read);
  if (sync_mode) {
    aprintf_stderr("Reading samples in sync mode...\n");
    while (!do_exit) {
      aprintf("(%s:%d) bytes_to_read = %lu", __FILE__, __LINE__, bytes_to_read);
      r = rtlsdr_read_sync(dev, buffer, out_block_size, &n_read);
      if (r < 0) {
        aprintf_stderr("WARNING: sync read failed.\n");
        break;
      }

      if ((bytes_to_read > 0) && (bytes_to_read < (uint32_t)n_read)) {
        aprintf("(%s:%d) n_read = %lu, bytes_to_read = %lu", __FILE__, __LINE__, n_read, bytes_to_read);
        n_read = bytes_to_read;
        do_exit = 1;
      }

      if (fwrite(buffer, 1, n_read, file) != (size_t)n_read) {
        aprintf_stderr("Short write, samples lost, exiting!\n");
        break;
      }
      else {
        aprintf("(%s:%d) n_read = %lu", __FILE__, __LINE__, n_read);
      }

      if ((uint32_t)n_read < out_block_size) {
        aprintf("n_read = %lu, out_block_size = %lu", n_read, out_block_size);
        aprintf_stderr("Short read, samples lost, exiting!\n");
        break;
      }

      if (bytes_to_read > 0)
        bytes_to_read -= n_read;
    }
  } else {
    aprintf_stderr("Reading samples in async mode...\n");
    r = rtlsdr_read_async(dev, rtlsdr_callback, (void *)file,
              0, out_block_size);
  }

  if (do_exit)
    aprintf_stderr("\nUser cancel, exiting...\n");
  else
    aprintf_stderr("\nLibrary error %d, exiting...\n", r);

  fclose(file);
  rtlsdr_fc_close(dev);
  free (buffer);

  announce_exceptioncode(com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_OK);
  pthread_mutex_lock(&running_mutex);
  is_running = 0;
  pthread_mutex_unlock(&running_mutex);
  aprintf("bye!");
  return;
}
