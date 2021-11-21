#ifndef CAMERA_H
#define CAMERA_H

#include "coords.h"
#include "world.h"

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
    int x;
    int y;
    float z;
    bool render;
} LayeredDisplayCoords;

typedef struct {
    LayeredDisplayCoords vertices[4];
    float zIndex;
    bool render;
} DisplayFace;

typedef struct {
    Block block;
    DisplayFace faces[6];
    bool render;
} DisplayBlock;

Camera camera_default();

DisplayCoords camera_orthToPersp(double x, double y, double distance, double fovDeg);
CartesianVector camera_worldSpaceToCameraSpace(CartesianVector vector, CartesianVector cameraPosition, PolarVector cameraHeading);
DisplayBlock camera_buildDisplayBlock(Camera camera, Block block);

void camera_moveInAriz(Camera *camera, double distance, double ariz);
void camera_render(Camera camera, World world);

#endif