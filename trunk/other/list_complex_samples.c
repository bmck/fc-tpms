// $Id$
// $(c)$

// extract_blocks_to_file.c
// Takes 4 positional arguments: the source file containing binary encoded complex numbers,
// the starting initial (included) block number (where a block is 256 samples), the final
// (included) block number, and the target file name.

// Compile using gcc:

// gcc list_complex_samples.c -o list_complex_samples

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "universal_defines.h"

int main(int argc, char **argv) {

  if (argc != 2) {
    printf("usage: %s (sourcefile.bin)\n", argv[0]);
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
  num_samples = (long)(ceil((double)(infilesize)/(double)(2.0*sizeof(unsigned char))));
  rewind(i);

  long j, k;
  float v[2];
  int b;
  char c[20];

  for (j = 0; j < num_samples; j++) {
    v[0] = 0.0; v[1] = 0.0;
    for (k = 0; k < 2; k++) {
      b = fgetc(i);
      if (feof(i)) break;
      v[k] = ((float)(b-127)/(float)128.0);
    }
    if (v[1] < 0.0)
      sprintf(c, "%f%fi", v[0], v[1]);
    else
      sprintf(c, "%f+%fi", v[0], v[1]);
    printf("%f, %f, \"%s\"\n", v[0], v[1], c);
  }

  printf("\n\n");
  fclose(i);
  return(0);
}