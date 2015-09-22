// $Id$
// $(c)$

// num_samples.c
// Takes one argument: the source file containing binary encoded complex numbers.
// Returns the number of samples in the file.

// Compile using gcc:

// gcc num_samples.c -o num_samples

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

  unsigned long infilesize, num_samples;
  fseek(i, 0L, SEEK_END);
  infilesize = (unsigned long) ftell(i);
  num_samples = (long)(ceil((double)(infilesize)/(double)(2.0*sizeof(float))));

  printf("%lu\n", num_samples);
  return(0);
}
