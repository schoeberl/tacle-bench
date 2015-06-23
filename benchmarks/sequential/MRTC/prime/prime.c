/* MDH WCET BENCHMARK SUITE. File version $Id: prime.c,v 1.5 2011-01-10 14:46:56 falk Exp $ */

/* Changes:
 * JG 2005/12/08: Prototypes added, and changed exit to retun in main.
 */

#include <machine/spm.h>
typedef  unsigned char  bool;
typedef  unsigned int   uint;

bool divides (uint n, uint m);
bool even (uint n);
bool prime (uint n);
void swap (uint* a, uint* b);


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

bool divides (uint n, uint m) {
  return (m % n == 0);
}

bool even (uint n) {
  return (divides (2, n));
}

bool prime (uint n) {
  uint i;
  if (even (n))
      return (n == 2);
  _Pragma("loopbound min 73 max 357")
  for (i = 3; i * i <= n; i += 2) { 
      if (divides (i, n)) /* ai: loop here min 0 max 357 end; */
          return 0; 
  }
  return (n > 1);
}

void swap (uint* a, uint* b) {
  uint tmp = *a;
  *a = *b; 
  *b = tmp;
}

int main (void) {

volatile _SPM unsigned int *cyc_ptr_low = (volatile _SPM unsigned int *) 0xF0020004;
volatile _SPM unsigned int *cyc_ptr_high = (volatile _SPM unsigned int *) 0xF0020000;

  unsigned long long cyc_ptr_low_saved = (unsigned long long)*cyc_ptr_low;
  unsigned long long cyc_ptr_high_saved = (unsigned long long)*cyc_ptr_high;

  unsigned long long start = cyc_ptr_low_saved | (cyc_ptr_high_saved<<32);
  uint x =  21649;
  uint y = 513239;
  swap (&x, &y);


  cyc_ptr_low_saved = (unsigned long long)*cyc_ptr_low;
  cyc_ptr_high_saved = (unsigned long long)*cyc_ptr_high;

  unsigned long long end = cyc_ptr_low_saved | (cyc_ptr_high_saved<<32);
  printstats(start, end);

  return (!(prime(x) && prime(y)));
}

