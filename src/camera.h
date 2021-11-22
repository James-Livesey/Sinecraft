#ifndef CAMERA_H
#define CAMERA_H

#include "coords.h"
#include "world.h"

#define SWAPCOORDS(a, b) {DisplayCoords temp = a; a = b; b = temp;}

enum {
    FACE_NX = 0,
    FACE_PX = 1,
    FACE_NY = 2,
    FACE_PY = 3,
    FACE_NZ = 4,
    FACE_PZ = 5
};

typedef struct {
    CartesianVector position;
    PolarVector heading;
    int fov;
} Camera;

typedef struct {
    int x;
    int y;
    bool render;
} DisplayCoords;

typedef struct {
    DisplayCoords vertices[4];
    double z;
} DisplayBlockFace;

typedef struct {
    unsigned int count;
    DisplayBlockFace* faces;
} DisplayBlockFaces;

Camera camera_default();

DisplayCoords camera_orthToPersp(double x, double y, double distance, double fovDeg);
CartesianVector camera_worldSpaceToCameraSpace(CartesianVector vector, CartesianVector cameraPosition, PolarVector cameraHeading);

void camera_moveInAriz(Camera *camera, double distance, double ariz);
void camera_render(Camera camera, World world);

#endif