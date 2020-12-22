#ifndef KERNEL_FIXED_POINT
#define KERNEL_FIXED_POINT
#include <stdint.h>
#define FRACTION 16384  // 2^14

struct real{
    int64_t value;
};

void real_init(struct real * r, int value);

struct real convert_tofixed_point(int x);       /* */

int convert_todecimal_rounding_toward_zero(struct real x);      /* */ 

int convert_todecimal_rounding_toward_nearset(struct real x);       /* */

struct real add(struct real x, struct real y);          /* */

struct real subtract(struct real x, struct real y);     /* */

struct real multiply(struct real x, struct real y);     /* */

struct real divide(struct real x, struct real y);       /* */

#endif /* kernel/fixed_point.h */
