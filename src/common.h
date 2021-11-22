#ifndef COMMON_H_
#define COMMON_H_

#include "lut.h"

#define PI 3.14159265358979323846
#define SQUARE(x) (x * x)
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define LERP(v0, v1, t) ((v0 * (1 - t)) + (v1 * t))
#define INVLERP(v0, v1, v) ((v - v0) / (v1 - v0))

int common_degrees(double rad);
double common_radians(int deg);

double common_sin(int deg);
double common_cos(int deg);
double common_tan(int deg);

int common_asin(double value);
int common_acos(double value);
int common_atan(double value);

#endif