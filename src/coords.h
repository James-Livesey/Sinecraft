#ifndef COORDS_H_
#define COORDS_H_

typedef struct {
    double x;
    double y;
    double z;
} CartesianVector;

typedef struct {
    double r;
    double incl;
    double ariz;
} PolarVector;

CartesianVector coords_defaultCartesian();
CartesianVector coords_addCartesian(CartesianVector a, CartesianVector b);
CartesianVector coords_fromPolar(PolarVector polar);
double coords_getMagnitude(CartesianVector vector);

PolarVector coords_defaultPolar();
PolarVector coords_addPolar(PolarVector a, PolarVector b);
PolarVector coords_fromCartesian(CartesianVector cartesian);

#endif