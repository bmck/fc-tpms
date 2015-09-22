// $Id$
// $(c)$

// extract_binary_samples_to_file.c
// Takes 4 positional arguments: the source file containing binary encoded complex numbers,
// the starting initial (included) block number (where a block is 256 samples), the final
// (included) block number, and the target file name.

// Compile using gcc:

// gcc extract_binary_samples_to_file.c -o extract_binary_samples_to_file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "universal_defines.h"

int main(int argc, char **argv) {

  if (argc != 5) {
    printf("usage: %s (sourcefile) (firstsample#) (lastsample#) (targetfile)\n", argv[0]);
    exit(1);
  }

  char sourcefile[200], targetfile[200];
  strcpy(sourcefile, argv[1]);
  strcpy(targetfile, argv[4]);

  long firstsample, lastsample;
  firstsample = atol(argv[2]);
  lastsample = atol(argv[3]);

  FILE *i, *o;
  if ((i = fopen(sourcefile, "rb")) == NULL) {
    printf("Sourcefile %s could not be opened for reading.\n", sourcefile);
    exit(2);
  }
  if ((o = fopen(targetfile, "wb")) == NULL) {
    printf("Targetfile %s could not be opened for writing.\n", targetfile);
    exit(3);
  }

  fseek(i, (firstsample * 2), SEEK_SET);
  long num_samples;
  num_samples = (lastsample - firstsample + 1);

  if (num_samples <= 0) {
    printf("The firstsample (%ld) must be less than or equal to the lastsample (%ld).\n", firstsample, lastsample);
    exit(4);
  }

  unsigned char dat[2 * num_samples];
  fread(dat, sizeof(unsigned char), 2 * num_samples, i);
  fwrite(dat, sizeof(unsigned char), 2 * num_samples, o);

  printf("\n\n");
  fclose(i);
  fclose(o);
  exit(0);
}