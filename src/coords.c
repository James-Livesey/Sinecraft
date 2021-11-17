#include <math.h>

#include "coords.h"

#define SQUARE(x) (x * x)

CartesianVector coords_defaultCartesian() {
    return CartesianVector {.x = 0, .y = 0, .z = 0};
}

CartesianVector coords_addCartesian(CartesianVector a, CartesianVector b) {
    return CartesianVector {.x = a.x + b.x, .y = a.y + b.y, .z = a.z + b.z};
}

CartesianVector coords_fromPolar(PolarVector polar) {
    return CartesianVector {
        .x = polar.r * sin(polar.incl) * cos(polar.ariz),
        .y = polar.r * cos(polar.incl),
        .z = polar.r * sin(polar.incl) * sin(polar.ariz)
    };
}

double coords_getMagnitude(CartesianVector vector) {
    return sqrt(SQUARE(vector.x) + SQUARE(vector.y) + SQUARE(vector.z));
}

PolarVector coords_defaultPolar() {
    return PolarVector {.r = 0, .incl = M_PI, .ariz = 0};
}

PolarVector coords_addPolar(PolarVector a, PolarVector b) {
    return PolarVector {.r = a.r + b.r, .incl = a.incl + b.incl, .ariz = a.ariz + b.ariz};
}

PolarVector coords_fromCartesian(CartesianVector cartesian) {
    return PolarVector {
        .r = coords_getMagnitude(cartesian),
        .incl = atan(sqrt(SQUARE(cartesian.x) + SQUARE(cartesian.z)) / cartesian.y),
        .ariz = cartesian.x == 0 ? M_PI / 2 : atan(cartesian.z / cartesian.x) + (cartesian.x < 0 ? M_PI : 0)
    };
}