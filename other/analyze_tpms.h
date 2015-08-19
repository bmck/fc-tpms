// $Id$
// $(c)$

#ifndef __LIQUID_ANALYSIS_H
#define __LIQUID_ANALYSIS_H

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

int get_success();
char *get_app_dir();

unsigned int update_state(complex float x, complex float prev_x, unsigned int sample_num); //, struct vsf_session* p_sess);

char *fleet_analysis(char *fn);

void init_analysis();
void end_analysis();
int analyze_file(char *src_filename);

unsigned long get_dec_address_val( /*char *bit_vals, long num_bits*/ );
char *get_hex_address_str( /*char *bit_vals, long num_bits,*/ char *returned_url);
long get_temp_c( /*char *bit_vals, long num_bits*/ );
long get_temp_f( /*char *bit_vals, long num_bits*/ );
double get_pressure_kpa( /*char *bit_vals, long num_bits*/ );
double get_pressure_psi( /*char *bit_vals, long num_bits */ );
char *get_url( /*long addr, long press, long temp,*/ char *returned_url);

#endif