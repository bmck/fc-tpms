// $Id$
// $(c)$

#ifndef __STATE_MACHINE_H
#define __STATE_MACHINE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "universal_defines.h"

#define BITS_PER_MSG BITS_PER_FREESCALE_MSG

#define AVG_SAMPLES_PER_HF_CYCLE 27
#define MIN_EXPECTED_SAMPLES_PER_HF_CYCLE (int)(0.85*AVG_SAMPLES_PER_HF_CYCLE)
#define MAX_EXPECTED_SAMPLES_PER_HF_CYCLE ((int)(1.15*AVG_SAMPLES_PER_HF_CYCLE)+1)
#define LO_HFCYCLE_SAMPLES MIN_EXPECTED_SAMPLES_PER_HF_CYCLE
#define HI_HFCYCLE_SAMPLES MAX_EXPECTED_SAMPLES_PER_HF_CYCLE
#define PRECEDING_HF_CYCLE(z) (LO_HFCYCLE_SAMPLES <= (z)) && (samples_in_cycle <= (z))

#define PRELUDE_SIZE 1000

#define PEAK_VAL (unsigned char)0xFF
#define MID_VAL (unsigned char)0x7F

#define PEAK +1
#define UNK 0
#define NEG -1

#define NO_PKT -999
#define HF_PRELUDE -1

void save_sample_rate(uint32_t);
int analyze_input(unsigned char *, uint32_t, uint32_t);
void analyze_within_no_pkt_state(unsigned char, uint32_t, uint32_t);
void analyze_within_prelude_state(unsigned char, uint32_t, uint32_t);
void analyze_within_bit0_state(unsigned char, uint32_t, uint32_t);
void analyze_within_bit1_state(unsigned char, uint32_t, uint32_t);
void analyze_within_bit_i_state(unsigned char, uint32_t, uint32_t);

int get_success();

unsigned long get_dec_address_val( /*char *bit_vals, long num_bits*/ );
char *get_hex_address_str( /*char *bit_vals, long num_bits,*/ char *returned_url);
long get_temp_c( /*char *bit_vals, long num_bits*/ );
long get_temp_f( /*char *bit_vals, long num_bits*/ );
double get_pressure_kpa( /*char *bit_vals, long num_bits*/ );
double get_pressure_psi( /*char *bit_vals, long num_bits */ );
char *get_url( /*long addr, long press, long temp,*/ char *returned_url);

#endif