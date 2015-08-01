// $Id$
// $(c)$

// power_attrs_per_block.c
// Takes at least one argument: the source file containing binary encoded complex numbers.
// Beyond that, additional arguments are expected to be an ordered list of attributes that are
// provided for each block in the order in which they are provided as a CSV list.  This list
// can be easily sorted using the *NIX sort function.
// (included) block number, and the target file name.

// Compile using gcc:

// gcc -o power_attrs_per_block power_attrs_per_block.c fft.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "universal_defines.h"
#include "fft.h"

// get_complexblock_from_iqfile converts a block of 256 IQ samples from infile
// into an array of 512 interleaved floats that represent the real and
// imaginary components of corresponding complex samples
float *get_complexblock_from_iqfile(FILE *i, long block_num, float *returned_f) {
  int b, j;
  fseek(i, block_num * 256 * 2, SEEK_SET);
  for (j = 0; j < IQ_SAMPLES_PER_BLOCK * 2; j++) {
    b = fgetc(i);
    if (feof(i)) break;
    returned_f[j] = ((float)(b-127)/(float)128.0);
  }
  fclose(i);
  return returned_f;
}


int main(int argc, char **argv) {

  if (argc < 2) {
    printf("usage: %s (sourcefile) (attr1) (attr2) ...\n", argv[0]);
    printf("supported attributes are: num, time, sum, max, avg, & spread\n");
    exit(1);
  }

  char sourcefile[200];
  strcpy(sourcefile, argv[1]);

  FILE *i;
  if ((i = fopen(sourcefile, "rb")) == NULL) {
    printf("Sourcefile %s could not be opened for reading.\n", sourcefile);
    exit(2);
  }

  fseek(i, 0L, SEEK_END);
  unsigned long binfilesize; binfilesize = (unsigned long) ftell(i);
  long num_blocks; num_blocks = (long)(ceil((double)binfilesize/(double)(2.0 * sizeof(float) * IQ_SAMPLES_PER_BLOCK)));
  long block_num;

  float returned_f[2*IQ_SAMPLES_PER_BLOCK];
  struct complex cplx_f[IQ_SAMPLES_PER_BLOCK];
  int j;

  for (block_num = 0L; block_num < num_blocks; block_num++) {
    get_complexblock_from_iqfile(i, block_num, returned_f);
    for (j = 0; j < IQ_SAMPLES_PER_BLOCK; j++) { // weighted w coeffs for Hann window
      cplx_f[j].real = returned_f[j*2] * 0.5 * (1.0 - cos(2.0*PI*j/(double)(IQ_SAMPLES_PER_BLOCK - 1)));
      cplx_f[j].imag = returned_f[j*2+1] * 0.5 * (1.0 - cos(2.0*PI*j/(double)(IQ_SAMPLES_PER_BLOCK - 1)));
    }
    fft(1, (long)log2((double)IQ_SAMPLES_PER_BLOCK), cplx_f);

    double block_abs[IQ_SAMPLES_PER_BLOCK];
    double block_max, block_sum; block_max = -1.0; block_sum = 0.0;
    for (j = 0; j < IQ_SAMPLES_PER_BLOCK; j++) {
      block_abs[j] = (double) sqrt(cplx_f[j].real*cplx_f[j].real + cplx_f[j].imag*cplx_f[j].imag);
      if (block_abs[j] > block_max) block_max = block_abs[j];
      block_sum += block_abs[j];
    }
    double block_avg; block_avg = block_sum / (double)IQ_SAMPLES_PER_BLOCK;
    double block_spread; block_spread = block_max / block_avg;

    for (j = 2; j < argc; j++) {
      if (strcmp(argv[j], "num") == 0)
        printf("%ld", block_num);
      else if (strcmp(argv[j], "time") == 0)
        printf("%lf", ((double)(block_num * IQ_SAMPLES_PER_BLOCK)/(double)SAMPLE_RATE));
      else if (strcmp(argv[j], "sum") == 0)
        printf("%lf", block_sum);
      else if (strcmp(argv[j], "max") == 0)
        printf("%lf", block_max);
      else if (strcmp(argv[j], "avg") == 0)
        printf("%lf", block_avg);
      else if (strcmp(argv[j], "spread") == 0)
        printf("%lf", block_spread);
      if (j < argc-1)
        printf(", ");
    }
    printf("\n");
  }

  printf("\n\n");
  fclose(i);
  return(0);
}
