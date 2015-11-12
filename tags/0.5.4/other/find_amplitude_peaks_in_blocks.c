// $Id$
// $(c)$

// find_amplitude_peaks_in_blocks.c
// Takes 3 positional arguments: the source file containing binary encoded complex numbers,
// the starting initial (included) block number (where a block is 256 samples), and the final
// (included) block number.  The output will be a series of CSV formatted rows containing
// the matching sample numbers (counted from the beginning of the file), and the number of
// samples since the previous matching sample.

// Compile using gcc:

// gcc find_amplitude_peaks_in_blocks.c -o find_amplitude_peaks_in_blocks

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "universal_defines.h"

int main(int argc, char **argv) {

  if (argc != 4) {
    printf("usage: %s (sourcefile) (firstblock#) (lastblock#)\n", argv[0]);
    exit(1);
  }

  char sourcefile[200];
  strcpy(sourcefile, argv[1]);

  long firstblock, lastblock;
  firstblock = atol(argv[2]);
  lastblock = atol(argv[3]);

  FILE *i, *o;
  if ((i = fopen(sourcefile, "rb")) == NULL) {
    printf("Sourcefile %s could not be opened for reading.\n", sourcefile);
    exit(2);
  }

  float v1, v2, peak_threshold = 0.995;
  long j, num_samples, last_sample, prev, prev_sample;  prev = 0L;
  fseek(i, (j = IQ_SAMPLES_PER_BLOCK * firstblock), SEEK_SET);
  num_samples = (lastblock - firstblock + 1) * IQ_SAMPLES_PER_BLOCK;
  last_sample = (lastblock + 1) * IQ_SAMPLES_PER_BLOCK - 1;

  if (num_samples <= 0) {
    printf("The firstblock (%ld) must be less than or equal to the lastblock (%ld).\n", firstblock, lastblock);
    exit(4);
  }

  for (prev_sample = -1L ; !feof(i) && j < (last_sample + 1); j++) {
    fread((void*)(&v1), sizeof(v1), 1, i);
    fread((void*)(&v2), sizeof(v2), 1, i);

    if (v1 > peak_threshold) {
      if (prev == -1) {
        printf("%ld, ", j);
        if (prev_sample < 0L)
          printf("-1\n");
        else
          printf("%ld\n", j-prev_sample);
        prev_sample = j;
      }
      prev = 1;
    }
    else if (v1 < 0.0) {
      prev = -1;
    }
  }

  printf("\n\n");
  fclose(i);
  return(0);
}