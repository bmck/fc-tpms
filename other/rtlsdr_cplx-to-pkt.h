// $Id$
// $(c)$

#ifndef __RTLSDR_CPLX_TO_PKT_H
#define __RTLSDR_CPLX_TO_PKT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>

#include "universal_defines.h"
#include "fft.h"

void iqfile_to_cplxfile(char *infile, char *outfile);
float *get_complexblock_from_iqfile(char *infile, long block_num, float *returned_f);

long compute_ccindex_contain_sample(long samp, long *cc, long cc_length);

char *get_app_dir();
int get_success();

unsigned long get_dec_address_val( /*char *bit_vals, long num_bits*/ );
char *get_hex_address_str( /*char *bit_vals, long num_bits,*/ char *returned_url);
long get_temp_c( /*char *bit_vals, long num_bits*/ );
long get_temp_f( /*char *bit_vals, long num_bits*/ );
double get_pressure_kpa( /*char *bit_vals, long num_bits*/ );
double get_pressure_psi( /*char *bit_vals, long num_bits */ );
char *get_url( /*long addr, long press, long temp,*/ char *returned_url);

void find_pkt_in_binfile( );
void get_pkt( const char *infilename /*float sample_rate, const char *infilename, char *bits*/ );

short fft(short int dir, long m, struct complex *buffer);

#endif

