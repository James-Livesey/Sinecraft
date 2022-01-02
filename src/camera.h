#ifndef CAMERA_H
#define CAMERA_H

#include "coords.h"
#include "textures.h"
#include "world.h"

#define MAX_FACE_SELECT_DISTANCE 4
#define MAX_TEXTURE_RENDER_DISTANCE 6

#define SWAPCOORDS(a, b) {DisplayCoords temp = a; a = b; b = temp;}

enum {
    FACE_NX = 0,
    FACE_PX = 1,
    FACE_NY = 2,
    FACE_PY = 3,
    FACE_NZ = 4,
    FACE_PZ = 5
};

enum {
    BLOCK_PLACE_SUCCESS,
    BLOCK_PLACE_NOT_SELECTED,
    BLOCK_PLACE_NOT_PLACEABLE,
    BLOCK_PLACE_OUT_OF_BOUNDS,
    BLOCK_PLACE_NOT_VISIBLE
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
    Block* block;
    int face;
    DisplayCoords vertices[4];
    double z;
    int texture;
} DisplayBlockFace;

typedef struct {
    unsigned int count;
    DisplayBlockFace* faces;
} DisplayBlockFaces;

Camera camera_default();

DisplayCoords camera_orthToPersp(double x, double y, double distance, double fovDeg);
CartesianVector camera_worldSpaceToCameraSpace(CartesianVector vector, CartesianVector cameraPosition, PolarVector cameraHeading);

void camera_moveInAriz(Camera *camera, World world, double distance, double ariz);
void camera_render(Camera camera, World world);

Block camera_getSelectedBlock();
int camera_destroySelectedBlock(World* world);
int camera_placeBlockOnFace(World* world, Camera camera, unsigned int type);

#endif