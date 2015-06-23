//#include <stdio.h>
#include "wcclibm.h"
#include "snipmath.h"
#include <machine/spm.h>

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

int main(void)
{
volatile _SPM unsigned int *cyc_ptr_low = (volatile _SPM unsigned int *) 0xF0020004;
volatile _SPM unsigned int *cyc_ptr_high = (volatile _SPM unsigned int *) 0xF0020000;

  unsigned long long cyc_ptr_low_saved = (unsigned long long)*cyc_ptr_low;
  unsigned long long cyc_ptr_high_saved = (unsigned long long)*cyc_ptr_high;

  unsigned long long start = cyc_ptr_low_saved | (cyc_ptr_high_saved<<32);
  double  a1 = 1.0f, b1 = -10.5f, c1 = 32.0f, d1 = -30.0f;
  double  a2 = 1.0f, b2 = -4.5f, c2 = 17.0f, d2 = -30.0f;
  double  a3 = 1.0f, b3 = -3.5f, c3 = 22.0f, d3 = -31.0f;
  double  a4 = 1.0f, b4 = -13.7f, c4 = 1.0f, d4 = -35.0f;
  double  x[3];
  double X;
  int     solutions;
  int i;
  unsigned long l = 0x3fed0169L;
  struct int_sqrt q;
  long n = 0;

  /* solve soem cubic functions */
  /* should get 3 solutions: 2, 6 & 2.5   */
  SolveCubic(a1, b1, c1, d1, &solutions, x);

  SolveCubic(a2, b2, c2, d2, &solutions, x);

  SolveCubic(a3, b3, c3, d3, &solutions, x);

  SolveCubic(a4, b4, c4, d4, &solutions, x);

  /* Now solve some random equations */
  _Pragma( "loopbound min 5 max 5" )
  for(a1=1;a1<10;a1+=2) {
    _Pragma( "loopbound min 5 max 5" )
    for(b1=10;b1>0;b1-=2) {
      _Pragma( "loopbound min 7 max 7" )
      for(c1=5;c1<15;c1+=1.5f) {
        _Pragma( "loopbound min 5 max 5" )
        for(d1=-1;d1>-11;d1-=2) {
          SolveCubic(a1, b1, c1, d1, &solutions, x);
        }
      }
    }
  }

  /* perform some integer square roots */
  _Pragma( "loopbound min 1000 max 1000" )
  for (i = 1; i < 1001; i+=1) {
    usqrt(i, &q);
    // remainder differs on some machines
  }

  usqrt(l, &q);

  /* convert some rads to degrees */
  _Pragma( "loopbound min 361 max 361" )
  for (X = 0.0f; X <= 360.0f; X += 1.0f) {
    deg2rad(X);
  }

  _Pragma( "loopbound min 360 max 360" )
  for (X = 0.0f; X <= (2 * PI + 1e-6f); X += (PI / 180)) {
    rad2deg(X);
  }

  cyc_ptr_low_saved = (unsigned long long)*cyc_ptr_low;
  cyc_ptr_high_saved = (unsigned long long)*cyc_ptr_high;

  unsigned long long end = cyc_ptr_low_saved | (cyc_ptr_high_saved<<32);
  printstats(start, end);

  return 0;
}
