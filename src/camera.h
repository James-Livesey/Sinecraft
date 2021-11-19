#ifndef CAMERA_H
#define CAMERA_H

#include "coords.h"

typedef struct {
    CartesianVector position;
    PolarVector heading;
    int fov;
} Camera;

typedef struct {
    int x;
    int y;
} DisplayCoords;

Camera camera_default();

DisplayCoords camera_orthToPersp(double x, double y, double distance, double fovDeg);
CartesianVector camera_worldSpaceToCameraSpace(CartesianVector vector, CartesianVector cameraPosition, PolarVector cameraHeading);

void camera_moveInAriz(Camera *camera, double distance, double ariz);
void camera_render(Camera camera);

#endif