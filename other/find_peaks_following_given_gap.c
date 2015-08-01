// $Id$
// $(c)$

// find_peaks_following_given_gap.c
// Takes 3 positional arguments: the source file containing binary encoded complex numbers,
// the starting initial (included) block number (where a block is 256 samples), and the final
// (included) block number.  The output will be a series of CSV formatted rows containing
// the matching sample numbers (counted from the beginning of the file), and the number of
// samples since the previous matching sample.

// Compile using gcc:

// gcc find_peaks_following_given_gap.c -o find_peaks_following_given_gap

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "universal_defines.h"

int main(int argc, char **argv) {

  if (argc != 5) {
    printf("usage: %s (sourcefile) (firstblock#) (lastblock#) (apxgap)\n", argv[0]);
    exit(1);
  }

  char sourcefile[200];
  strcpy(sourcefile, argv[1]);

  long firstblock, lastblock, target_gap, target_gap_lo, target_gap_hi;
  firstblock = atol(argv[2]);
  lastblock = atol(argv[3]);
  target_gap = atol(argv[4]);
  target_gap_lo = (long) (0.9 * target_gap);
  target_gap_hi = (long) (ceil((double)(1.1 * target_gap)));

  FILE *i, *o;
  if ((i = fopen(sourcefile, "rb")) == NULL) {
    printf("Sourcefile %s could not be opened for reading.\n", sourcefile);
    exit(2);
  }

  float v1, v2, peak_threshold = 0.995;
  long curr_sample, num_samples, final_sample, prev_event, prev_sample;
  long curr_gap;

  prev_event = 0L;
  fseek(i, (curr_sample = IQ_SAMPLES_PER_BLOCK * firstblock), SEEK_SET);
  num_samples = (lastblock - firstblock + 1) * IQ_SAMPLES_PER_BLOCK;
  final_sample = (lastblock + 1) * IQ_SAMPLES_PER_BLOCK - 1;

  if (num_samples <= 0) {
    printf("The firstblock (%ld) must be less than or equal to the lastblock (%ld).\n", firstblock, lastblock);
    exit(4);
  }

  for (prev_sample = -1L ; !feof(i) && curr_sample < (final_sample + 1); curr_sample++) {
    fread((void*)(&v1), sizeof(v1), 1, i);
    fread((void*)(&v2), sizeof(v2), 1, i);

    if (v1 > peak_threshold) {
      if (prev_event == -1) {
        curr_gap = curr_sample - prev_sample;
        if ((curr_gap >= target_gap_lo) && (curr_gap <= target_gap_hi)) {
          printf("%ld, ", curr_sample);
          if (prev_sample < 0L)
            printf("-1, -1\n");
          else
            printf("%ld, %ld\n", prev_sample, curr_gap);
        }
        prev_sample = curr_sample;
      }
      prev_event = 1;
    }
    else if (v1 < 0.0) {
      prev_event = -1;
    }
  }

  printf("\n\n");
  fclose(i);
  return(0);
}