#ifndef CAMERA_H
#define CAMERA_H

#include "coords.h"

typedef struct {
    CartesianVector position;
    PolarVector heading;
    int fov;
    int maxRenderDistance;
} Camera;

Camera camera_default();
char camera_shootRay(Camera camera, PolarVector direction, CartesianVector position);

#endif