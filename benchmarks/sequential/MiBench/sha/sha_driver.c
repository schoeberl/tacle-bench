/* NIST Secure Hash Algorithm */

#include "my_file.h"
#include <machine/spm.h>
#include <stdio.h>
#include "sha.h"

extern unsigned char data[32743];

int main(void) {
volatile _SPM unsigned int *cyc_ptr_low = (volatile _SPM unsigned int *) 0xF0020004;
volatile _SPM unsigned int *cyc_ptr_high = (volatile _SPM unsigned int *) 0xF0020000;

  unsigned long long cyc_ptr_low_saved = (unsigned long long)*cyc_ptr_low;
  unsigned long long cyc_ptr_high_saved = (unsigned long long)*cyc_ptr_high;

  unsigned long long start = cyc_ptr_low_saved | (cyc_ptr_high_saved<<32);
    struct my_FILE fin;
    fin.data = data;
    fin.size = 32743;  // set size = 3247552 for input_large
    fin.cur_pos = 0;
    struct SHA_INFO sha_info;
    sha_stream(&sha_info, &fin);
        
  cyc_ptr_low_saved = (unsigned long long)*cyc_ptr_low;
  cyc_ptr_high_saved = (unsigned long long)*cyc_ptr_high;

  unsigned long long end = cyc_ptr_low_saved | (cyc_ptr_high_saved<<32);
printf("%llu %llu %llu\n", start, end, end-start);
    return 0;
}

