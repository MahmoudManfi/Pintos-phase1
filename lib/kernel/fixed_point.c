#include <fixed_point.h>


void real_init(struct real * r, int value) {
    r->value = (int64_t) value*FRACTION;
}

struct real convert_tofixed_point(int x){
    struct real r; 
    real_init(&r,x);
    return r;
}

int convert_todecimal_rounding_toward_zero(struct real r){
    return r.value/FRACTION;
}

int convert_todecimal_rounding_toward_nearset(struct real r){
    if(r.value >= 0) return (r.value + FRACTION / 2) / FRACTION;
    if(r.value <= 0) return (r.value - FRACTION / 2) / FRACTION;
}

struct real add(struct real x, struct real y){
    struct real r;
    r.value = x.value+y.value;
    return r;
}

struct real subtract(struct real x, struct real y){
    struct real r;
    r.value=x.value-y.value;
    return r;
}

struct real multiply(struct real x, struct real y){
    struct real r;
    r.value = x.value * y.value / FRACTION;
    return r;
}

struct real divide(struct real x, struct real y){
    struct real r;
    r.value = x.value * FRACTION / y.value;
    return r;
}
