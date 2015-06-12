#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

// rtlsdr-to-gqrx Copyright 2014 Paul Brewer KI6CQ
// License: GNU GPL 3.0
//
// IQ file converter for Software Defined Radio Programs rtl_sdr, gqrx
// from rtl_sdr recording format -- interleaved unsigned char
// to gqrx/gnuradio .cfile playback format -- complex64
//
// compile on Linux with:  gcc ./rtlsdr-to-gqrx.c -o rtlsdr-to-gqrx
//
// For further usage instructions, see my post at:
// http://ham.stackexchange.com/q/2113/70
//

int main(int argc, char *argv[])
{
  // long j = 0L;
  int byte1, byte2;  // int -- not unsigned char -- see fgetc man page
  float _Complex fc;
  const size_t fc_size = sizeof(fc);
  FILE *infile,*outfile;
  const float scale = 1.0/128.0;
  const char *infilename = argv[1];
  const char *outfilename = argv[2];
  if (argc<3){
    printf("usage:  rtlsdr-to-gqrx infile outfile\n");
    exit(1);
  }
  // printf("in= %s out= %s \n", infilename, outfilename);
  infile=fopen(infilename,"rb");
  outfile=fopen(outfilename,"wb");
  if ((infile==NULL) || (outfile==NULL)){
    printf("Error opening files\n");
    exit(1);
  }
  while ((byte1=fgetc(infile)) != EOF){
    // printf("%lu\n", ftell(infile));
    if ((byte2=fgetc(infile)) == EOF){
      exit(0);
    }
    fc = scale*(byte1-127) + I*scale*(byte2-127);
    fwrite(&fc,fc_size,1,outfile);
    // printf("%ld ... %d, %lu, %d, %lu, %f + i%f\n", j, byte1, sizeof(byte1), byte2, sizeof(byte2), creal(fc), cimag(fc));
    // j++;
  }
  return 0;
}
