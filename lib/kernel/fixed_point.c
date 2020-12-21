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

int64_t add(int64_t x, int64_t y){
    return x+y;
}

int64_t subtract(int64_t x, int64_t y){
    return x-y;
}

int64_t multiply(int64_t x, int64_t y){
    return x * y / FRACTION;
}

int64_t divide(int64_t x, int64_t y){
    return  x * FRACTION / y;
}
