#ifndef PATMOS_TACLE_H_
#define PATMOS_TACLE_H_
//#include <machine/spm.h>

#define _SPM __attribute__((address_space(1)))

volatile _SPM unsigned int *cyc_ptr_low = (volatile _SPM unsigned int *) 0xF0020004;
unsigned long long cyc_ptr_low_start;
unsigned long long cyc_ptr_low_end;

void start_count() {
    cyc_ptr_low_start = (unsigned long long)*cyc_ptr_low;
}

void end_count() {
    cyc_ptr_low_end = (unsigned long long)*cyc_ptr_low;
}

void printchar( unsigned char c) {
    volatile _SPM unsigned int *uart_rec = (volatile _SPM unsigned int *) 0xF0080004;
    volatile _SPM unsigned int *uart_s = (volatile _SPM unsigned int *) 0xF0080000;

    // wait for status ready
    while( (*uart_s & 1) == 0 ) {}

    *uart_rec = c;
}


void printstats(unsigned long long start, unsigned long long end) {
    unsigned long long diff = end - start;
    unsigned char buf[32] = {0};
    int i = 0, j = 0;
    buf[0] = start % 10;
    while((start /= 10) != 0) {
        i++;
        buf[i] = start % 10;
    }

    for(j=i; j>-1; j--) {
        printchar( '0' + buf[j] );
        buf[j] = 0;
    }
    printchar(' ');

    i = 0;
    j = 0;

    buf[0] = end % 10;
    while((end /= 10) != 0) {
        i++;
        buf[i] = end % 10;
    }

    for(j=i; j>-1; j--) {
        printchar( '0' + buf[j] );
        buf[j] = 0;
    }
    printchar(' ');

    i = 0;
    j = 0;

    buf[0] = diff % 10;
    while((diff /= 10) != 0) {
        i++;
        buf[i] = diff % 10;
    }

    for(j=i; j>-1; j--) {
        printchar( '0' + buf[j] );
        buf[j] = 0;
    }
    printchar('\n');
    

}

#endif
