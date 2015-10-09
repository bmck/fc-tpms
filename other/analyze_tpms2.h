// $Id$
// $(c)$

#ifndef __ANALYZE_TPMS2_H
#define __ANALYZE_TPMS2_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "universal_defines.h"
#include "liquid.h"

#define BITS_PER_FREESCALE_MSG 69
#define BITS_PER_MSG BITS_PER_FREESCALE_MSG

#define UNK 0


char *fleet_analysis(char *fn);

int analyze_file(char *src_filename);
int *get_packet();
bool validate_packet(int *packet_syms, int num_syms);
bool is_valid_prelude(unsigned long file_loc);
unsigned long find_prelude_start();
int *try_to_get_packet_symbols(unsigned int pkt_start, int num_syms, unsigned int est_samples_per_sym);
int *get_packet_symbols(unsigned int pkt_start, int num_bits, unsigned int samples_per_sym);
int get_second_symbol(unsigned int sym_start, unsigned int samples_per_sym);
int get_symbol(unsigned int sym_start, unsigned int samples_per_sym);
int get_bit(unsigned long bit_start, unsigned long bit_end);
int scan_samples_for_eob_transition(float complex *samples, int num_samples);
int *convert_samples_to_binary(complex float *samples, int num_samples);

unsigned long get_current_read_loc();
float complex *get_some_samples(int num_samples, unsigned long start_sample_location);
float complex *get_next_sample();

int get_success();
unsigned long get_dec_address_val();
char *get_hex_address_str(char *returned_url);
long get_temp_c();
long get_temp_f();
double get_pressure_kpa();
double get_pressure_psi();
// char *get_url( /*long addr, long press, long temp,*/ char *returned_url);


#endif