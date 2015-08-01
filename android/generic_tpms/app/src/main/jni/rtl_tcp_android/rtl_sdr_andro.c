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
static uint32_t bytes_read = 0;

static char rtl_filename[200];

void init_all_variables() {
	dev = NULL;
	global_numq = 0;
	llbuf_num = 500;
	bytes_to_read = 0;
	bytes_read = 0;
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
		LOGE("(%s:%d) Signal caught, exiting! (signal %d)\n", __FILE__, __LINE__, signum);
		announce_exceptioncode(com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_SIGNAL_CAUGHT);
	}

	do_exit = 1;
}

// void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx)
// {
// 	if (!do_exit) {

// 		struct llist *rpt = (struct llist*)malloc(sizeof(struct llist));
// 		rpt->data = (char*)malloc(len);
// 		memcpy(rpt->data, buf, len);
// 		rpt->len = len;
// 		rpt->next = NULL;
// 		bytes_read += len;

// 		pthread_mutex_lock(&ll_mutex);

// 		if (ll_buffers == NULL) {
// 			ll_buffers = rpt;
// 		} else {
// 			struct llist *cur = ll_buffers;
// 			int num_queued = 0;

// 			while (cur->next != NULL) {
// 				cur = cur->next;
// 				num_queued++;
// 			}

// 			if (llbuf_num && llbuf_num == num_queued - 2) {
// 				struct llist *curelem;

// 				free(ll_buffers->data);
// 				curelem = ll_buffers->next;
// 				free(ll_buffers);
// 				ll_buffers = curelem;
// 			}

// 			cur->next = rpt;

// 			global_numq = num_queued;
// 		}
// 		pthread_cond_signal(&cond);
// 		pthread_mutex_unlock(&ll_mutex);
// 	}
// }

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

void rtlsdr_main(int usbfd, const char * uspfs_path_input, int argc, char **argv)
{
//  char *rtl_filename = NULL;
	int n_read;
	int r, opt;
	int test_mode = 0;
	int gain = 0;
	int ppm_error = 0;
	int sync_mode = 1;
	int direct_sampling = 0;
	int dithering = 1;
	FILE *file;
	uint8_t *buffer;
	int dev_index = 0;
	int dev_given = 0;
	uint32_t frequency = 100000000;
	uint32_t samp_rate = DEFAULT_SAMPLE_RATE;
	uint32_t out_block_size = DEFAULT_BUF_LENGTH;

//	FILE *cplx_file;
//	float *cplx_buf;
	unsigned char *cbuf;
	struct sigaction sigact;

	pthread_mutex_lock(&running_mutex);
	is_running = 1;
	pthread_mutex_unlock(&running_mutex);

	if (usbfd != -1 && (!(fcntl(usbfd, F_GETFL) != -1 || errno != EBADF))) {
		LOGE("(%s:%d) Invalid file descriptor %d, - %s", __FILE__, __LINE__, usbfd, strerror(errno));
		announce_exceptioncode(com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_INVALID_FD);
		pthread_mutex_lock(&running_mutex);
		is_running = 0;
		pthread_mutex_unlock(&running_mutex);
		return;
	}

	init_all_variables();

	// LOGI("argc = %d\n", argc);
	// for (r = 0; r < argc; r++) {
	//   LOGI("argv[%d] = %s\n",r, argv[r]);
	// }

	optind = 0;

	while ((opt = getopt(argc, argv, "t:d:f:g:s:b:n:p:D:SN")) != -1) {
		switch (opt) {
		case 't':
			test_mode = (int)(atof(optarg));
			test_mode = (test_mode == 0) ? 0 : 1;
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
			bytes_to_read = (uint32_t)atofs(optarg) * 2;
			break;
		case 'S':
			// sync_mode = 1;
			break;
		case 'D':
			direct_sampling = atoi(optarg);
			break;
		case 'N':
			dithering = 0;
			break;
		default:
			LOGE("(%s:%d) Unexpected argument '%c' with value '%s' received as an argument\n", __FILE__, __LINE__, opt, optarg);
			announce_exceptioncode(com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_WRONG_ARGS);
			pthread_mutex_lock(&running_mutex);
			is_running = 0;
			pthread_mutex_unlock(&running_mutex);
			return;
		}
	}

	if (argc < optind) {
		LOGE("(%s:%d) Expected at least %d arguments, but got %d\n", __FILE__, __LINE__, optind, argc);
		announce_exceptioncode(com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_WRONG_ARGS);
		pthread_mutex_lock(&running_mutex);
		is_running = 0;
		pthread_mutex_unlock(&running_mutex);
		return;
	} else {
		strcpy(rtl_filename, argv[optind]);
	}

	if (out_block_size < MINIMAL_BUF_LENGTH ||
	        out_block_size > MAXIMAL_BUF_LENGTH ) {
		LOGE("(%s:%d) Output block size wrong value, falling back to default\n", __FILE__, __LINE__);
		LOGE("Minimal length: %u\n", MINIMAL_BUF_LENGTH);
		LOGE("Maximal length: %u\n", MAXIMAL_BUF_LENGTH);
		out_block_size = DEFAULT_BUF_LENGTH;
	}

	buffer = malloc(out_block_size * sizeof(uint8_t));
	cbuf = buffer;
//	cplx_buf = malloc(out_block_size * sizeof(float));

	sigact.sa_handler = sighandler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	sigaction(SIGINT, &sigact, NULL);
	sigaction(SIGTERM, &sigact, NULL);
	sigaction(SIGQUIT, &sigact, NULL);
	sigaction(SIGPIPE, &sigact, NULL);

	r = 0;
	if (test_mode == 0) {

		if (usbfd == -1) {
			if (!dev_given) {
				dev_index = verbose_device_search("0");
			}

			if (dev_index < 0) {
				LOGE("(%s:%d) No supported devices found.\n", __FILE__, __LINE__);
				announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_NO_DEVICES );
				pthread_mutex_lock(&running_mutex);
				is_running = 0;
				pthread_mutex_unlock(&running_mutex);
				return;
			}

			r = rtlsdr_open(&dev, (uint32_t)dev_index);
		} else {
			LOGI("(%s:%d) Opening device with fd %d at %s\n", __FILE__, __LINE__, usbfd, uspfs_path_input);
			r = rtlsdr_open2(&dev, dev_index, usbfd, uspfs_path_input);
		}

		if (r < 0) {
			LOGE("(%s:%d) Failed to open rtlsdr device #%d.\n", __FILE__, __LINE__, dev_index);
			announce_exceptioncode( r );
			pthread_mutex_lock(&running_mutex);
			is_running = 0;
			pthread_mutex_unlock(&running_mutex);
			return;
		}

		if (NULL == dev) {
			LOGE("(%s:%d) Failed to open rtlsdr device #%d.\n", __FILE__, __LINE__, dev_index);
			announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_FAILED_TO_OPEN_DEVICE );
			pthread_mutex_lock(&running_mutex);
			is_running = 0;
			pthread_mutex_unlock(&running_mutex);
			return;
		}

		LOGI("(%s:%d) bytes_to_read = %lu\n", __FILE__, __LINE__, (unsigned long)bytes_to_read);

		if (!dithering) {
			LOGI("(%s:%d) Disabling dithering... \n", __FILE__, __LINE__);
			r = rtlsdr_set_dithering(dev, dithering);
			if (r) {
				LOGI("(%s:%d) failure\n", __FILE__, __LINE__);
			} else {
				LOGI("(%s:%d) success\n", __FILE__, __LINE__);
			}
		}

		if (direct_sampling) {
			verbose_direct_sampling(dev, direct_sampling);
		}

		/* Set the sample rate */
		r = rtlsdr_set_sample_rate(dev, samp_rate);
		if (r < 0) {
			LOGE("(%s:%d) WARNING: Failed to set sample rate.\n", __FILE__, __LINE__);
			announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_NOT_ENOUGH_POWER );
			pthread_mutex_lock(&running_mutex);
			is_running = 0;
			pthread_mutex_unlock(&running_mutex);
			return;
		}
		else {
			save_sample_rate(samp_rate);
		}

		/* Set the frequency */
		r = rtlsdr_set_center_freq(dev, frequency);
		if (r < 0)
			LOGE("(%s:%d) WARNING: Failed to set center freq.\n", __FILE__, __LINE__);
		else
			LOGI("(%s:%d) Tuned to %i Hz.", __FILE__, __LINE__, frequency);

		if (0 == gain) {
			/* Enable automatic gain */
			r = rtlsdr_set_tuner_gain_mode(dev, 0);
			if (r < 0)
				LOGE("(%s:%d) WARNING: Failed to enable automatic gain.\n", __FILE__, __LINE__);
		} else {
			/* Enable manual gain */
			r = rtlsdr_set_tuner_gain_mode(dev, 1);
			if (r < 0)
				LOGE("(%s:%d) WARNING: Failed to enable manual gain.\n", __FILE__, __LINE__);

			/* Set the tuner gain */
			r = rtlsdr_set_tuner_gain(dev, gain);
			if (r < 0)
				LOGE("(%s:%d) WARNING: Failed to set tuner gain.\n", __FILE__, __LINE__);
			else
				LOGE("(%s:%d) Tuner gain set to %f dB\n", __FILE__, __LINE__, gain / 10.0);
		}

		verbose_ppm_set(dev, ppm_error);

		if (strcmp(rtl_filename, "-") == 0) {
			LOGE("(%s:%d) WARNING: Failed to set output rtl_filename.\n", __FILE__, __LINE__);
			announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_RTLSDR_FILENAME_NOT_SPECIFIED );
			pthread_mutex_lock(&running_mutex);
			is_running = 0;
			pthread_mutex_unlock(&running_mutex);
			return;
		} else {
			LOGI("(%s:%d) rtl_filename = %s\n", __FILE__, __LINE__, rtl_filename);
			file = fopen(rtl_filename, "wb");
			if (!file) {
				LOGE("(%s:%d) Failed to open %s\n", __FILE__, __LINE__, rtl_filename);
				announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_RTLSDR_FILENAME_NOT_SPECIFIED );
				pthread_mutex_lock(&running_mutex);
				is_running = 0;
				pthread_mutex_unlock(&running_mutex);
				return;
			}
			rewind(file);

//			strcpy(&(rtl_filename[strlen(rtl_filename) - 3]), "cfile");
//			LOGI("(%s:%d) rtl_filename = %s\n", __FILE__, __LINE__, rtl_filename);
//
//			cplx_file = fopen(rtl_filename, "wb");
//			LOGI("(%s:%d) rtl_filename = %s\n", __FILE__, __LINE__, rtl_filename);
//			if (!cplx_file) {
//				LOGE("(%s:%d) Failed to open %s\n", __FILE__, __LINE__, rtl_filename);
//				announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_RTLSDR_FILENAME_NOT_SPECIFIED );
//				pthread_mutex_lock(&running_mutex);
//				is_running = 0;
//				pthread_mutex_unlock(&running_mutex);
//				return;
//			}
		}

		/* Reset endpoint before we start reading from it (mandatory) */
		verbose_reset_buffer(dev);

		// if (sync_mode) {
			LOGE("(%s:%d) Reading samples in sync mode...\n", __FILE__, __LINE__);
			while (!do_exit) {
				r = rtlsdr_read_sync(dev, buffer, out_block_size, &n_read);
				if (r < 0) {
					LOGE("(%s:%d) WARNING: sync read failed.\n", __FILE__, __LINE__);
					break;
				}

				if ((bytes_to_read > 0) && (bytes_to_read < (uint32_t)n_read)) {
					n_read = bytes_to_read;
					do_exit = 1;
				}

				if (fwrite(buffer, 1, n_read, file) != (size_t)n_read) {
					LOGE("(%s:%d) Short write, samples lost, exiting!\n", __FILE__, __LINE__);
					break;
				}

//				for (r = 0; r < n_read; r++) {
//					cplx_buf[r] = (((float)(cbuf[r]) - 127.0) / (float)128.0);
//				}
//				if (fwrite(cplx_buf, sizeof(float), n_read, cplx_file) != (size_t)n_read) {
//					LOGE("(%s:%d) Short write, samples lost, exiting!\n", __FILE__, __LINE__);
//					break;
//				}

				if ((uint32_t)n_read < out_block_size) {
					LOGE("(%s:%d) Short read, samples lost, exiting!\n", __FILE__, __LINE__);
					break;
				}

				if (bytes_to_read > 0)
					bytes_to_read -= n_read;

				int k;
//				k = analyze_input(cplx_buf, n_read, bytes_read, rtl_filename);
				k = analyze_input(buffer, n_read, bytes_read);
				LOGI("(%s:%d) n_read = %d, bytes_read = %lu, analyze_input = %d\n", __FILE__, __LINE__, n_read, (long unsigned int)bytes_read, k);
				do_exit |= k; // FleetCents processing hook
				bytes_read += n_read;

			}
		// } else {
		// 	LOGE("(%s:%d) Reading samples in async mode...\n", __FILE__, __LINE__);
		// 	r = rtlsdr_read_async(dev, rtlsdr_callback, (void *)file,
		// 	                      0, out_block_size);
		// }

		if (do_exit)
			LOGE("(%s:%d) User cancel, exiting...\n", __FILE__, __LINE__);
		else
			LOGE("(%s:%d) Library error %d, exiting...\n", __FILE__, __LINE__, r);

		fclose(file);
//		fclose(cplx_file);
		rtlsdr_close(dev);
		free (buffer);



	}


	else {

		// test_mode = 1


		if (strcmp(rtl_filename, "-") == 0) {
			LOGE("(%s:%d) WARNING: Failed to set input binary filename.\n", __FILE__, __LINE__);
			announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_RTLSDR_FILENAME_NOT_SPECIFIED );
			pthread_mutex_lock(&running_mutex);
			is_running = 0;
			pthread_mutex_unlock(&running_mutex);
			return;
		} else {
			LOGI("(%s:%d) binary filename = %s\n", __FILE__, __LINE__, rtl_filename);
			file = fopen(rtl_filename, "rb");
			if (!file) {
				LOGE("(%s:%d) Failed to open %s\n", __FILE__, __LINE__, rtl_filename);
				announce_exceptioncode( com_fleetcents_generic_1tpms_core_RtlSdr_RTLSDR_FILENAME_NOT_SPECIFIED );
				pthread_mutex_lock(&running_mutex);
				is_running = 0;
				pthread_mutex_unlock(&running_mutex);
				return;
			}
			rewind(file);

//			char cfile_filename[200];
//			strcpy(cfile_filename, rtl_filename);
//			strcpy(&(cfile_filename[strlen(cfile_filename) - 3]), "cfile");
//			LOGI("(%s:%d) cfile filename = %s\n", __FILE__, __LINE__, cfile_filename);
//			int create_cplx_file = 0;
//
//			cplx_file = fopen(cfile_filename, "rb");
//			if (!cplx_file) {
//				LOGI("(%s:%d) %s not found; will create.\n", __FILE__, __LINE__, cfile_filename);
//				create_cplx_file = 1;
//			}
//			else {
//				LOGI("(%s:%d) what length is %s?\n", __FILE__, __LINE__, cfile_filename);
//				fseek(cplx_file, 0L, SEEK_END);
//				int lr; lr = ftell(cplx_file);
//				LOGI("(%s:%d) length is %lu.\n", __FILE__, __LINE__, (unsigned long)lr);
//				if (lr == 0)
//					create_cplx_file = 1;
//				fclose(cplx_file);
//			}
//
//			if (create_cplx_file == 1) {
//				LOGI("(%s:%d) Creating %s\n", __FILE__, __LINE__, cfile_filename);
//				fseek(file, 0L, SEEK_END);
//				bytes_to_read = ftell(file);
//				rewind(file);
//
//				cplx_file = fopen(rtl_filename, "wb");
//				for ( ; bytes_to_read > 0; bytes_to_read -= n_read, bytes_read += n_read) {
//					n_read = MIN(n_read, out_block_size);
//					fread(buffer, sizeof(unsigned char), n_read, file);
//					for (r = 0; r < n_read; r++) {
//						cplx_buf[r] = (((float)(cbuf[r]) - 127.0) / (float)128.0);
//					}
//					fwrite(cplx_buf, sizeof(float), n_read, cplx_file);
//				}
//				fclose(cplx_file);
//				LOGI("(%s:%d) Created %s\n", __FILE__, __LINE__, cfile_filename);
//			}

//			analyze_file(cfile_filename);
			analyze_file(rtl_filename);
		}
	}



	announce_exceptioncode(com_fleetcents_generic_1tpms_core_RtlSdr_EXIT_OK);
	pthread_mutex_lock(&running_mutex);
	is_running = 0;
	pthread_mutex_unlock(&running_mutex);
	LOGI("(%s:%d) bye!\n", __FILE__, __LINE__);
	return;
}
