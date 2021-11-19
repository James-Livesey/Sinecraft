#ifndef COMMON_H_
#define COMMON_H_

#define PI 3.14159265358979323846
#define LERP(v0, v1, t) ((v0 * (1 - t)) + (v1 * t))
#define INVLERP(v0, v1, v) ((v - v0) / (v1 - v0))

#endif