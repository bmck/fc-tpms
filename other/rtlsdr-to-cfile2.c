// $Id$
// $(c)$

#include <stdio.h>
#include <stdlib.h>

// Based on
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

float iq_to_cplx(int b)
{
  return ((float)(b-127)/(float)128.0);
}

void iqfile_to_cplxfile(char *infile, char *outfile)
{
  int b;
  float f;
  FILE *i, *o;
  i = fopen(infile, "rb");
  o = fopen(outfile, "wb");
  do {
    b = fgetc(i);
    if (feof(i)) break;
    f = iq_to_cplx(b);
    fwrite((void*) &f, sizeof(f), 1, o);
  } while (1);
}