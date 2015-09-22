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

void set_signal_nco_frequency(nco_crcf pll_nco, int curr_state);
bool is_pll_locked(void);
bool lock_pll(nco_crcf nco, float complex x);
void check_pll_lock(nco_crcf pll_nco, int curr_state, float complex x);
unsigned int update_state(complex float x, int b, unsigned int sample_num);

void init_analysis();
void end_analysis();
int analyze_input(unsigned char *buf, uint32_t len, uint32_t sample_offset);
int analyze_file(char *src_filename);
int analyze_iq_pairs(unsigned char *buf, uint32_t buf_length);

unsigned long get_dec_address_val( /*char *bit_vals, long num_bits*/ );
char *get_hex_address_str( /*char *bit_vals, long num_bits,*/ char *returned_url);
long get_temp_c( /*char *bit_vals, long num_bits*/ );
long get_temp_f( /*char *bit_vals, long num_bits*/ );
double get_pressure_kpa( /*char *bit_vals, long num_bits*/ );
double get_pressure_psi( /*char *bit_vals, long num_bits */ );
char *get_url( /*long addr, long press, long temp,*/ char *returned_url);

#endif