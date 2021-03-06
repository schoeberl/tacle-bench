/*
   FILE NAME:   bits.h

   TITLE:       Include file of package for work with bits

   DESCRIPTION:
       This is header file contains macros and the ANSI C prototype
       definitions for the package for work with bits and bit strings
       and C++ class for work with bits and bit strings.  A bit is
       given by address (start address) of byte from which counting
       bits starts and its displacement which is any non negative
       number of bit from the start address.  The most significant bit
       of the start address byte has number 0.  The bit string is
       given by its first bit and its length in bits.

   SPECIAL CONSIDERATION:
         Defining macro `NDEBUG' (e.g. by option `-D' in C/C++
       compiler command line) before the package macros usage disables
       to fix some internal errors and errors of usage of the package.

*/

#ifndef __BITS__
#define __BITS__

#define NDEBUG

#ifdef HAVE_LIMITS_H
#include <limits.h>
#else
#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif
#ifndef UCHAR_MAX
#define UCHAR_MAX 255
#endif
#ifndef SCHAR_MAX
#define SCHAR_MAX 127
#endif
#ifndef SCHAR_MIN
#define SCHAR_MIN (-128)
#endif
#ifndef USHRT_MAX
#define USHRT_MAX 65535
#endif
#ifndef SHRT_MAX
#define SHRT_MAX 32767
#endif  
#ifndef SHRT_MIN
#define SHRT_MIN (-32768)
#endif
#ifndef UINT_MAX
#define UINT_MAX (INT_MAX * 2U + 1)
#endif
#ifndef INT_MAX
#define INT_MAX 2147483647
#endif  
#ifndef INT_MIN
#define INT_MIN (-INT_MAX-1)
#endif
#endif


/* This macro value returns bit vlaue (0 or 1) with given bit
   displacement (0, 1, ...).  The macro has side effects!  Value of
   `bit_displacement' must be nonegative and can be greater than
   CHAR_BIT. */

#ifndef NDEBUG
#define BIT(start_byte, bit_displacement)\
  (((bit_displacement) < 0 ? abort (), 0: 0),\
   (((const char *) (start_byte)) [(bit_displacement) / CHAR_BIT]\
    >> (CHAR_BIT - 1 - (bit_displacement) % CHAR_BIT)) & 1)
#else
#define BIT(start_byte, bit_displacement)\
  ((((const char *) (start_byte)) [(bit_displacement) / CHAR_BIT]\
    >> (CHAR_BIT - 1 - (bit_displacement) % CHAR_BIT)) & 1)
#endif


/* This macro value sets up new value (must be `0' or `1') of a given
   bit (bit displacement starts with 0).  The macro has side effects!
   Value of `bit_displacement' must be nonegative and can be greater
   than CHAR_BIT. */

#ifndef NDEBUG
#define SET_BIT(start_byte, bit_displacement, bit)\
  (((bit_displacement) < 0 || ((bit) != 0 && (bit) != 1)\
    ? abort (), 0: 0),\
   ((char *) (start_byte)) [(bit_displacement) / CHAR_BIT]\
   = (((char *) (start_byte)) [(bit_displacement) / CHAR_BIT]\
      & ~(1 << (CHAR_BIT - 1 - (bit_displacement) % CHAR_BIT)))\
     | ((bit) << (CHAR_BIT - 1 - (bit_displacement) % CHAR_BIT)))
#else
#define SET_BIT(start_byte, bit_displacement, bit)\
  (((char *) (start_byte)) [(bit_displacement) / CHAR_BIT]\
   = (((char *) (start_byte)) [(bit_displacement) / CHAR_BIT]\
      & ~(1 << (CHAR_BIT - 1 - (bit_displacement) % CHAR_BIT)))\
     | ((bit) << (CHAR_BIT - 1 - (bit_displacement) % CHAR_BIT)))
#endif

int is_zero_bit_string (const void *start_byte, int bit_displacement,
                        int bit_length);
void bit_string_set (void *start_byte, int bit_displacement, int bit,
                     int bit_length);
void bit_string_copy (void *to, int to_bit_displacement,
                      const void *from, int from_bit_displacement,
                      int bit_length);
void bit_string_move (void *to, int to_bit_displacement,
                      const void *from, int from_bit_displacement,
                      int bit_length);
int bit_string_comparison (const void *str1, int bit_displacement1,
                           const void *str2, int bit_displacement2,
                           int bit_length);

#endif /* #ifndef __BITS__ */
