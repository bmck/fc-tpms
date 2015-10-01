// $Id$
// $(c)$

// state_machine_tester.c
// Takes one argument: the source file containing binary encoded unsigned chars collected
// by rtl-sdr.  This functions will run that file through the state machine and determine
// if/when a valid packet is found.

// Compile using gcc:

// gcc -o state_machine_tester state_machine_tester.c analyze_tpms.c /Users/bmck/src/liquid-dsp/libliquid.a -I/Users/bmck/src/liquid-dsp/include

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "universal_defines.h"
#include "analyze_tpms.h"

#define DEFAULT_BUF_LENGTH    (16 * 16384)
#define MINIMAL_BUF_LENGTH    512
#define MAXIMAL_BUF_LENGTH    (256 * 16384)

// static volatile int do_exit = 0;

char *get_url( /*long addr, long press, long temp,*/ char *returned_url) {
 // INIT_LOGGING
 char *url = malloc(sizeof(char) * 2048);
 returned_url = url;

 time_t rawtime; time ( &rawtime );
 struct tm * ptm; ptm = gmtime ( &rawtime );

 unsigned long addr = get_dec_address_val();
 long press = get_pressure_psi();
 long tempc = get_temp_c();

 sprintf(url, "http://198.36.127.105/tire_samples/create?tire_sample[sensor_id]=%ld&tire_sample[receiver_id]=%d&tire_sample[psi]=%ld&tire_sample[tempc]=%ld&tire_sample[sample_time]=%d-%02d-%02d%%20%02d:%02d:%02d",
         addr, 8, press, tempc, ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
 // LOGI("%s: (%s:%d) url = %s\n", src_name, __FILE__, __LINE__,  url);
 return returned_url;
}

int main(int argc, char **argv) {

  if (argc != 2) {
    printf("usage: %s (sourcefile.bin)\n", argv[0]);
    exit(1);
  }

  char sourcefile[200];
  strcpy(sourcefile, argv[1]);

  // FILE *i;
  // if ((i = fopen(sourcefile, "rb")) == NULL) {
  //   printf("Sourcefile %s could not be opened for reading.\n", sourcefile);
  //   exit(2);
  // }

  // uint32_t out_block_size = DEFAULT_BUF_LENGTH;
  // uint8_t *buffer;
  // uint32_t n_read = 0L;
  // uint32_t bytes_read;
  // buffer = malloc(out_block_size * sizeof(uint8_t));

  // while (!feof(i) && !do_exit) {
  //   bytes_read = fread((void*)(buffer), sizeof(uint8_t), out_block_size, i);

  //   do_exit |= analyze_input(buffer, bytes_read, n_read);

  //   n_read += bytes_read;
  // }


  analyze_file(sourcefile);

  if (get_success()) {
    char addr[10]; strcpy(addr, "");
    printf("address: %s \n", get_hex_address_str(addr));

    printf("tempC: %ld \n", get_temp_c());

    printf("pressure (kPa): %lf \n", get_pressure_kpa());

    char url[300]; strcpy(url, "");
    printf("url: >%s< \n", get_url(url));
  }
  else {
    printf("packet extraction unsuccessful\n");
  }
  return(0);
}
