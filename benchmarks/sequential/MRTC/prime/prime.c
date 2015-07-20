/* MDH WCET BENCHMARK SUITE. File version $Id: prime.c,v 1.5 2011-01-10 14:46:56 falk Exp $ */

/* Changes:
 * JG 2005/12/08: Prototypes added, and changed exit to retun in main.
 */

#include "../../../include/patmos.h"
typedef  unsigned char  bool;
typedef  unsigned int   uint;

bool divides (uint n, uint m);
bool even (uint n);
bool prime (uint n);
void swap (uint* a, uint* b);

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

  start_count();

  uint x =  21649;
  uint y = 513239;
  swap (&x, &y);

  int result = !(prime(x) && prime(y));
  
  end_count();
  printstats(cyc_ptr_low_start, cyc_ptr_low_end);

  return result;
}

