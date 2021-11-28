#include <math.h>

#include "common.h"
#include "lut.h"

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

// See https://math.stackexchange.com/a/1098585
int common_atan(double value) {
    if (value > 1) {
        return 90 - common_atan(1 / value);
    }

    if (value < 0) {
        return -common_atan(-value);
    }

    return value * (45 - ((value - 1) * (14 + (3.83 * value))));
}

int common_atan2(double y, double x) {
    if (x == 0) {
        if (y > 0) {
            return 90;
        }

        if (y < 0) {
            return -90;
        }

        return 0; // Undefined
    }

    if (x > 0) {
        return common_atan(y / x);
    }

    if (y >= 0) {
        return common_atan(y / x) + 180;
    }

    return common_atan(y / x) - 180;
}

// See https://stackoverflow.com/a/7380529
int common_asin(double value) {
    return common_atan2(value, sqrt((1 + value) * (1 - value)));
}

// See https://stackoverflow.com/a/7380529
int common_acos(double value) {
    return common_atan2(sqrt((1 + value) * (1 - value)), value);
}