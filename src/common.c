#include <math.h>

#include "common.h"
#include "lut.h"

int common_degrees(double rad) {
    return (rad / PI) * 180;
}

double common_radians(int deg) {
    return (deg / 180) * PI;
}

double common_sin(int deg) {
    if (deg < 0) {
        return -common_sin(-deg);
    }

    deg = deg % 360;

    if (deg > 90 && deg <= 180) {
        return common_sin(180 - deg);
    }

    if (deg > 180) {
        return -common_sin(deg - 180);
    }

    return (double)LUT_SIN[deg] / (double)0xFFFF;
}

double common_cos(int deg) {
    return common_sin(90 - deg);
}

double common_tan(int deg) {
    return common_sin(deg) / common_cos(deg);
}

int common_asin(double value) {
    return common_degrees(asin(value));
}

int common_acos(double value) {
    return common_degrees(acos(value));
}

int common_atan(double value) {
    return common_degrees(atan(value));
}