#include <fixed_point.h>


int64_t convert_tofixed_point(int x){
    return (int64_t) x*FRACTION;
}

int convert_todecimal_rounding_toward_zero(int64_t x){
    return x/FRACTION;
}

int convert_todecimal_rounding_toward_nearset(int64_t x){
    if(x >= 0) return (x + FRACTION / 2) / FRACTION;
    if(x <= 0) return (x - FRACTION / 2) / FRACTION;
}


int add(int x, int y){
    x = convert_tofixed_point(x);
    y = convert_tofixed_point(y);
    int z = convert_todecimal_rounding_toward_nearset(x+y);
    return z;
}

int subtract(int x, int y){
    x = convert_tofixed_point(x);
    y = convert_tofixed_point(y);
    int z = convert_todecimal_rounding_toward_nearset(x-y);
    return z;
}


int multiply(int x, int y){
    x = convert_tofixed_point(x);
    y = convert_tofixed_point(y);
    int z = convert_todecimal_rounding_toward_nearset(((int64_t) x) * y / FRACTION);
    return z;
}


int divide(int x, int y){
    x = convert_tofixed_point(x);
    y = convert_tofixed_point(y);
    int z = convert_todecimal_rounding_toward_nearset(((int64_t) x) * FRACTION / y);
    return z;
}