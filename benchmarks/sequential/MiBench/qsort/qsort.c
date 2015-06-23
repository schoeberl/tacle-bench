#include "wcclibm.h"
#include "qsort.h"
#include "wccstdlib.h"
#include <machine/spm.h>

#define NUM_STRINGS 681
#define NUM_VECTORS 1000

extern const char *input_string[NUM_STRINGS];
extern const unsigned int input_vector[NUM_VECTORS*3];


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

int main( void ) {
  volatile _SPM unsigned int *cyc_ptr_low = (volatile _SPM unsigned int *) 0xF0020004;
  volatile _SPM unsigned int *cyc_ptr_high = (volatile _SPM unsigned int *) 0xF0020000;

  unsigned long long cyc_ptr_low_saved = (unsigned long long)*cyc_ptr_low;
  unsigned long long cyc_ptr_high_saved = (unsigned long long)*cyc_ptr_high;

  unsigned long long start = cyc_ptr_low_saved | (cyc_ptr_high_saved<<32);
  struct my3DVertexStruct vectors[NUM_VECTORS];
  char strings[NUM_STRINGS][STRING_LENGTH];
  unsigned int i, j;
  unsigned int x,y,z;
  unsigned int read_counter = 0;

  // Init arrays
  _Pragma( "loopbound min 681 max 681" )
  for ( i = 0; i < NUM_STRINGS; i++ ) {
    _Pragma( "loopbound min 2 max 13" )
    for ( j = 0; j < STRING_LENGTH - 1; j++ ) {
      strings[i][j] = input_string[i][j];
      if ( input_string[i][j] == '\0' ) {
        break;
      }
    }
    // Terminate with '\0' anyways
    strings[i][STRING_LENGTH - 1] = '\0';
  }
  _Pragma( "loopbound min 1000 max 1000" )
  for ( i = 0; i < NUM_VECTORS; i++ ) {
    x = vectors[i].x = input_vector[read_counter++];
	  y = vectors[i].y = input_vector[read_counter++];
	  z = vectors[i].z = input_vector[read_counter++];
	  vectors[i].distance = sqrt( pow( x, 2 ) + pow( y, 2 ) + pow( z, 2 ) );
  }
  _Pragma( "marker recursivecall" )
  _Pragma( "flowrestriction 1*qsorts_strings <= 521*recursivecall" )
  qsorts_strings(*strings, NUM_STRINGS, sizeof(char[STRING_LENGTH]));

  _Pragma( "marker recursivecall2" )
  _Pragma( "flowrestriction 1*qsorts_vectors <= 650*recursivecall2" )
  qsorts_vectors( (char*)vectors, NUM_VECTORS, sizeof(struct my3DVertexStruct) );

  cyc_ptr_low_saved = (unsigned long long)*cyc_ptr_low;
  cyc_ptr_high_saved = (unsigned long long)*cyc_ptr_high;

  unsigned long long end = cyc_ptr_low_saved | (cyc_ptr_high_saved<<32);
  printstats(start, end);

  return 0;
}
