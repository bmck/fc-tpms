// $Id$
// $(c)$

// num_blocks.c
// Takes one argument: the source file containing binary encoded complex numbers.
// Returns the number of blocks in the file.

// Compile using gcc:

// gcc num_blocks.c -o num_blocks

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "universal_defines.h"

int main(int argc, char **argv) {

  if (argc != 2) {
    printf("usage: %s (sourcefile)\n", argv[0]);
    exit(1);
  }

  char sourcefile[200];
  strcpy(sourcefile, argv[1]);

  FILE *i;
  if ((i = fopen(sourcefile, "rb")) == NULL) {
    printf("Sourcefile %s could not be opened for reading.\n", sourcefile);
    exit(2);
  }

  unsigned long infilesize, num_blocks;
  fseek(i, 0L, SEEK_END);
  infilesize = (unsigned long) ftell(i);
  num_blocks = (long)(ceil((double)(infilesize/(2.0*sizeof(float)*IQ_SAMPLES_PER_BLOCK))));

  printf("%lu\n", num_blocks);
  return(0);
}
