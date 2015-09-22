//
// rtlsdr_to_periodogram.c : convert RTL-SDR output file to periodogram
//
// Compile using gcc:

// gcc -o rtlsdr_to_periodogram_mod rtlsdr_to_periodogram_mod.c /Users/bmck/src/liquid-dsp/libliquid.a -I/Users/bmck/src/liquid-dsp/include

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "liquid.h"
#include "getopt.h"

// process output? or print raw samples
#define PROCESS 1

// print usage/help message
void usage()
{
	printf("%s [options]\n", __FILE__);
	printf("  h      : print help\n");
	printf("  f<file>: input data filename\n");
}

int main(int argc, char*argv[])
{
	// char       filename_input[256] = "mckinnon-manchester-19-Jul-2015/fc-abridged.bin";
	char       filename_input[256] = "/tmp/fc1.bin";
	const char filename_output[]   = "rtlsdr_to_periodogram.m";

	// create mod/demod objects
	int dopt;
	while ((dopt = getopt(argc, argv, "hf:")) != EOF) {
		switch (dopt) {
		case 'h':   usage();        return 0;
		case 'f':
			strncpy(filename_input, optarg, 255);
			filename_input[255] = '\0';
			break;
		default:
			exit(1);
		}
	}

	// create spectrogram
	unsigned int      nfft  = 12000;
	float             alpha = 1e-2f;
	float             psd[nfft];
	unsigned long int total_samples_in  = 0;
	unsigned long int total_samples_out = 0;
	uint8_t           buf2[2];
	float complex     bias =  0.0011370 + 0.0043801 * _Complex_I;

	// complex-valued samples
	spgramcf periodogram = spgramcf_create_kaiser(nfft, nfft / 2, 8.0f);

	// open input file
	FILE * fid = fopen(filename_input, "r");
	if (fid == NULL) {
		fprintf(stderr, "error: could not open %s for reading\n", filename_input);
		exit(-1);
	}

	FILE * fid2 = fopen(filename_output, "w");
	if (fid2 == NULL) {
		fprintf(stderr, "error: could not open %s for reading\n", filename_output);
		exit(-1);
	}
	fprintf(fid2, "%% %s : auto-generated file from %s\n", filename_output, __FILE__);
	fprintf(fid2, "clear all;\n");
	fprintf(fid2, "close all;\n");

	// preprocessing
	float Fs              = 2048.0f; // sample rate [kHz]
	float fsk_symbol_rate = 10.0f * (1.0f - 0.0089f) / Fs; //
	float fsk_freq_offset = -102.4 / Fs; // -0.05 = -1/20
	unsigned int k = 20;    // samples per symbol
	unsigned int p = 2700000;     // starting sample

	// objects
	iirfilt_crcf  filter  = iirfilt_crcf_create_lowpass(7, 0.05);
	msresamp_crcf presamp = msresamp_crcf_create(2.0, 60.f);
	nco_crcf      nco     = nco_crcf_create(LIQUID_VCO);
	msresamp_crcf resamp  = msresamp_crcf_create(k * fsk_symbol_rate, 60.0f);
	freqdem       fmdemod = freqdem_create(1.0f);
	firfilt_rrrf  mf      = firfilt_rrrf_create_kaiser(4 * k + 1, 2.0f / (float)k, 60.0f, 0.0f);

	// properties
	nco_crcf_set_frequency(nco, fsk_freq_offset);
	firfilt_rrrf_set_scale(mf, 0.5f);
	float complex buf_resamp[16];
	float         buf_demod[16];
	unsigned int  num_resamp;

	// While there are frames in the input file, read them, process
	// them and write them to the output file.
	do
	{
		// try to read 2 samples at a time
		if (fread(buf2, sizeof(uint8_t), 2, fid) != 2)
			break;

		//
		uint8_t xi = buf2[0];
		uint8_t xq = buf2[1];

		// convert to float complex type
		float complex x;
		float complex orig_x = ((float)(xi) - 127.0f) +
		                       ((float)(xq) - 127.0f) * _Complex_I;

		// scale and remove bias
		orig_x = orig_x / 128.0f - bias;

#if PROCESS
		// apply pre-processing
		iirfilt_crcf_execute(filter, orig_x, &orig_x);

		// remove carrier offset
		nco_crcf_mix_down(nco, orig_x, &orig_x);
		nco_crcf_step(nco);

		msresamp_crcf_execute(presamp, &orig_x, 1, x, &num_resamp);

		if (num_resamp > 0) {
			// resample
			msresamp_crcf_execute(resamp, &x, 1, buf_resamp, &num_resamp);

			// apply frequency discriminator
			freqdem_demodulate_block(fmdemod, buf_resamp, num_resamp, buf_demod);

			// apply 'matched' filter
			firfilt_rrrf_execute_block(mf, buf_demod, num_resamp, buf_demod);
#else
		buf_resamp[0] = x;
		num_resamp = 1;
#endif

			// accumulate spectrum
			spgramcf_accumulate_psd(periodogram, buf_resamp, alpha, num_resamp);
			total_samples_in++;

			// save resulting sample(s)
			unsigned int i;
			for (i = 0; i < num_resamp; i++) {
				fprintf(fid2, "x(end+1) = %12.4e +1i*%12.4e;\n", crealf(buf_resamp[i]), cimagf(buf_resamp[i]));
#if PROCESS
				fprintf(fid2, "y(end+1) = %12.4e;\n", buf_demod[i]);
				if ((total_samples_out % k) == 0)
					printf("%u", buf_demod[i] > 0.0f ? 1 : 0);
				total_samples_out++;
#endif
			}
		} while (!feof(fid) && total_samples_in < 50000L);
	}
#if PROCESS
	printf("\n");
#endif

	// close input file
	fclose(fid);

	// destroy allocated objects
	iirfilt_crcf_destroy(filter);
	nco_crcf_destroy(nco);
	msresamp_crcf_destroy(resamp);
	freqdem_destroy(fmdemod);
	firfilt_rrrf_destroy(mf);

	// write accumulated PSD
	spgramcf_write_accumulation(periodogram, psd);
	spgramcf_destroy(periodogram);

	printf("total samples in : %lu\n", total_samples_in);
	printf("total samples out: %lu\n", total_samples_out);

	// write output spectrum to file
	fprintf(fid2, "nfft = %u;\n", nfft);
	unsigned int i;
	for (i = 0; i < nfft; i++)
		fprintf(fid2, "psd(%6u) = %12.8f;\n", i + 1, psd[i]);
	fprintf(fid2, "figure;\n");
	fprintf(fid2, "subplot(2,1,1)\n");
	fprintf(fid2, "  n = length(x);\n");
	fprintf(fid2, "  t = 1:n;\n");
	fprintf(fid2, "  hold on;\n");
	fprintf(fid2, "  plot(t,real(x),'Color',[0 0.3 0.5],'LineWidth',1);\n");
	fprintf(fid2, "  plot(t,imag(x),'Color',[0 0.5 0.3],'LineWidth',1);\n");
	fprintf(fid2, "  hold off;\n");
	fprintf(fid2, "  axis([1000 1500 -1.2 1.2]);\n");
	fprintf(fid2, "  xlabel('Sample index');\n");
	fprintf(fid2, "  ylabel('Output sample');\n");
	fprintf(fid2, "  grid on;\n");
	fprintf(fid2, "subplot(2,1,2)\n");
	fprintf(fid2, "  f = [0:(nfft-1)]/nfft - 0.5;\n");
	fprintf(fid2, "  plot(f,psd,'Color',[0 0.3 0.5],'LineWidth',1.5);\n");
	fprintf(fid2, "  psd_min = floor(min(psd)/10)*10;\n");
	fprintf(fid2, "  psd_max =  ceil(max(psd)/10)*10;\n");
	fprintf(fid2, "  axis([-0.5 0.5 psd_min psd_max]);\n");
	fprintf(fid2, "  xlabel('Normalized Frequency [f/F_s]');\n");
	fprintf(fid2, "  ylabel('Power Spectral Density [dB]');\n");
	fprintf(fid2, "  grid on;\n");
#if PROCESS
	fprintf(fid2, "figure;\n");
	fprintf(fid2, "  k = %u;\n", k);
	fprintf(fid2, "  p = 1:k:n;\n");
	fprintf(fid2, "  plot(1:n,y,p,y(p),'x');\n");
	fprintf(fid2, "  grid on;\n");
	fprintf(fid2, "  xlabel('sample index');\n");
	fprintf(fid2, "  ylabel('frequency(t)');\n");
#endif
	fclose(fid);
	printf("results written to '%s'\n", filename_output);

	return 0;
}

