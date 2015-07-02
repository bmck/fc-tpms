// $Id$
// $(c)$

// extract_blocks_to_file.c
// Takes 4 positional arguments: the source file containing binary encoded complex numbers,
// the starting initial (included) block number (where a block is 256 samples), the final
// (included) block number, and the target file name.

// Compile using gcc:

// gcc extract_blocks_to_file.c -o extract_blocks_to_file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "universal_defines.h"

int main(int argc, char **argv) {

  if (argc != 4) {
    printf("usage: %s (sourcefile) (firstblock#) (lastblock#) (targetfile)\n", argv[0]);
    exit(1);
  }

  char sourcefile[200], targetfile[200];
  strcpy(sourcefile, argv[1]);
  strcpy(targetfile, argv[4]);

  long firstblock, lastblock;
  firstblock = atol(argv[2]);
  lastblock = atol(argv[3]);

  FILE *i, *o;
  if ((i = fopen(sourcefile, "rb")) == NULL) {
    printf("Sourcefile %s could not be opened for reading.\n", sourcefile);
    exit(2);
  }
  if ((o = fopen(targetfile, "wb")) == NULL) {
    printf("Targetfile %s could not be opened for writing.\n", targetfile);
    exit(3);
  }

  fseek(i, (IQ_SAMPLES_PER_BLOCK * firstblock), SEEK_SET);
  long num_blocks;
  num_blocks = (lastblock - firstblock + 1);

  if (num_blocks <= 0) {
    printf("The firstblock (%ld) must be less than or equal to the lastblock (%ld).\n", firstblock, lastblock);
    exit(4);
  }

  float dat[2 * IQ_SAMPLES_PER_BLOCK * num_blocks];
  fread(dat, sizeof(float), 2 * IQ_SAMPLES_PER_BLOCK * (lastblock - firstblock + 1), i);
  fwrite(dat, sizeof(float), 2 * IQ_SAMPLES_PER_BLOCK * (lastblock - firstblock + 1), o);

  fclose(i);
  fclose(o);
  exit(0);
}