#include <math.h>

#include "coords.h"
#include "common.h"

CartesianVector coords_defaultCartesian() {
    return (CartesianVector) {0, 0, 0};
}

CartesianVector coords_scaleCartesian(CartesianVector vector, double scaleFactor) {
    return (CartesianVector) {vector.x * scaleFactor, vector.y * scaleFactor, vector.z * scaleFactor};
}

CartesianVector coords_addCartesian(CartesianVector a, CartesianVector b) {
    return (CartesianVector) {a.x + b.x, a.y + b.y, a.z + b.z};
}

CartesianVector coords_multiplyCartesian(CartesianVector a, CartesianVector b) {
    return (CartesianVector) {a.x * b.x, a.y * b.y, a.z * b.z};
}

CartesianVector coords_fromPolar(PolarVector polar) {
    double inclSin = common_sin(polar.incl);

    return (CartesianVector) {
        polar.r * inclSin * common_cos(polar.ariz),
        polar.r * common_cos(polar.incl),
        polar.r * inclSin * common_sin(polar.ariz)
    };
}

CartesianVector coords_rotateCartesian(CartesianVector vector, PolarVector rotation) {
    return coords_fromPolar(coords_addPolar(coords_fromCartesian(vector), rotation));
}

double coords_getMagnitude(CartesianVector vector) {
    return sqrt(SQUARE(vector.x) + SQUARE(vector.y) + SQUARE(vector.z));
}

PolarVector coords_defaultPolar() {
    return (PolarVector) {0, 0, 0};
}

PolarVector coords_scalePolar(PolarVector vector, double scaleFactor) {
    return (PolarVector) {vector.r * scaleFactor, vector.incl * scaleFactor, vector.ariz * scaleFactor};
}

PolarVector coords_addPolar(PolarVector a, PolarVector b) {
    return (PolarVector) {a.r + b.r, a.incl + b.incl, a.ariz + b.ariz};
}

PolarVector coords_fromCartesian(CartesianVector cartesian) {
    return (PolarVector) {
        coords_getMagnitude(cartesian),
        common_acos(cartesian.y / coords_getMagnitude(cartesian)),
        cartesian.x == 0 ? 90 : common_atan(cartesian.z / cartesian.x) + 180
    };
}