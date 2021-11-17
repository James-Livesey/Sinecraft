#include <math.h>

#include "coords.h"
#include "common.h"

#define SQUARE(x) (x * x)

CartesianVector coords_defaultCartesian() {
    return (CartesianVector) {0, 0, 0};
}

CartesianVector coords_addCartesian(CartesianVector a, CartesianVector b) {
    return (CartesianVector) {a.x + b.x, a.y + b.y, a.z + b.z};
}

CartesianVector coords_fromPolar(PolarVector polar) {
    return (CartesianVector) {
        polar.r * sin(polar.incl) * cos(polar.ariz),
        polar.r * cos(polar.incl),
        polar.r * sin(polar.incl) * sin(polar.ariz)
    };
}

double coords_getMagnitude(CartesianVector vector) {
    return sqrt(SQUARE(vector.x) + SQUARE(vector.y) + SQUARE(vector.z));
}

PolarVector coords_defaultPolar() {
    return (PolarVector) {0, PI, 0};
}

PolarVector coords_addPolar(PolarVector a, PolarVector b) {
    return (PolarVector) {a.r + b.r, a.incl + b.incl, a.ariz + b.ariz};
}

PolarVector coords_fromCartesian(CartesianVector cartesian) {
    return (PolarVector) {
        coords_getMagnitude(cartesian),
        atan(sqrt(SQUARE(cartesian.x) + SQUARE(cartesian.z)) / cartesian.y),
        cartesian.x == 0 ? PI / 2 : atan(cartesian.z / cartesian.x) + (cartesian.x < 0 ? PI : 0)
    };
}