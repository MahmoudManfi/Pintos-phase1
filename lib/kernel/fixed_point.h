#ifndef KERNEL_FIXED_POINT
#define KERNEL_FIXED_POINT
#include <stdint.h>
#define FRACTION 16384  // 2^14


int64_t convert_tofixed_point(int x);       /* */

int convert_todecimal_rounding_toward_zero(int64_t x);      /* */ 

int convert_todecimal_rounding_toward_nearset(int64_t x);       /* */



int add(int x, int y);          /* */

int subtract(int x, int y);     /* */

int multiply(int x, int y);     /* */


int divide(int x, int y);       /* */



#endif /* kernel/fixed_point.h */