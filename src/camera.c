#include <stdlib.h>
#include <math.h>
#include <gint/display.h>
#include <gint/display-fx.h>

#include "flags.h"
#include "camera.h"
#include "common.h"
#include "coords.h"
#include "world.h"
#include "profiling.h"

const int VIEWPORT_WIDTH = 128;
const int VIEWPORT_HEIGHT = 64;

const unsigned int FACE_VERTICES[] = {
    0, 1, 3, 2, // NX
    5, 4, 6, 7, // PX
    0, 1, 5, 4, // NY
    2, 3, 7, 6, // PY
    0, 4, 6, 2, // NZ
    1, 5, 7, 3  // PZ
};

int lastFov = 0;
int verticalFov = 0;

Camera camera_default() {
    return (Camera) {
        .position = coords_defaultCartesian(),
        .heading = coords_defaultPolar(),
        .fov = 60
    };
}

double orthToPersp2d(double value, double distance, double fov) {
    double v1 = distance * common_tan(fov / 2);
    double v0 = -v1;
    double t = INVLERP(v0, v1, value);

    return LERP(0, 1, t);
}

DisplayCoords camera_orthToPersp(double x, double y, double distance, double fov) {
    if (fov != lastFov) {
        lastFov = fov;
        verticalFov = 2 * common_atan((VIEWPORT_HEIGHT / 2) / ((VIEWPORT_WIDTH / 2) / common_tan(fov / 2)));
    }

    if (distance == 0) {
        return (DisplayCoords) {VIEWPORT_WIDTH / 2, VIEWPORT_HEIGHT / 2, false};
    }

    return (DisplayCoords) {
        orthToPersp2d(x, distance, fov) * VIEWPORT_WIDTH,
        orthToPersp2d(y, distance, verticalFov) * VIEWPORT_HEIGHT,
        true
    };
}

CartesianVector camera_worldSpaceToCameraSpace(CartesianVector vector, CartesianVector cameraPosition, PolarVector cameraHeading) {
    cameraHeading.ariz += 180;

    vector.x -= cameraPosition.x;
    vector.y -= cameraPosition.y;
    vector.z -= cameraPosition.z;

    cameraHeading.r *= -1;
    cameraHeading.incl *= -1;
    cameraHeading.ariz *= -1;

    if (vector.x < 0 && vector.z < 0) {
        cameraHeading.ariz += 180;
    }

    CartesianVector result = coords_rotateCartesian(vector, cameraHeading);

    if (vector.x < 0 && vector.z >= 0) {
        result.x *= -1;
        result.z *= -1;
    }

    return result;
}

void camera_moveInAriz(Camera* camera, double distance, double ariz) {
    camera->position = coords_addCartesian(camera->position, coords_fromPolar((PolarVector) {
        distance,
        90,
        ariz
    }));
}

CartesianVector getAdjacentBlockPosition(Block block, unsigned int face) {
    switch (face) {
        case FACE_NX:
            return coords_addCartesian(block.position, (CartesianVector) {-1, 0, 0});

        case FACE_PX:
            return coords_addCartesian(block.position, (CartesianVector) {1, 0, 0});

        case FACE_NY:
            return coords_addCartesian(block.position, (CartesianVector) {0, -1, 0});

        case FACE_PY:
            return coords_addCartesian(block.position, (CartesianVector) {0, 1, 0});

        case FACE_NZ:
            return coords_addCartesian(block.position, (CartesianVector) {0, 0, -1});

        case FACE_PZ:
            return coords_addCartesian(block.position, (CartesianVector) {0, 0, 1});
    }

    return block.position;
}

bool adjacentBlockIsFilled(World world, Block block, unsigned int face) {
    return world_getBlock(world, getAdjacentBlockPosition(block, face)).type != BLOCK_TYPE_AIR;
}

bool shouldRenderFace(Camera camera, CartesianVector* vertices, unsigned int face) {
    switch (face) {
        case FACE_NX:
            return camera.position.x <= vertices[FACE_VERTICES[face * 4]].x;

        case FACE_PX:
            return camera.position.x >= vertices[FACE_VERTICES[face * 4]].x;

        case FACE_NY:
            return camera.position.y <= vertices[FACE_VERTICES[face * 4]].y;

        case FACE_PY:
            return camera.position.y >= vertices[FACE_VERTICES[face * 4]].y;

        case FACE_NZ:
            return camera.position.z <= vertices[FACE_VERTICES[face * 4]].z;

        case FACE_PZ:
            return camera.position.z >= vertices[FACE_VERTICES[face * 4]].z;
    }

    return false;
}

void setRenderFaceStates(bool* faceStates, Camera camera, CartesianVector* vertices) {
    for (unsigned int i = 0; i < 6; i++) {
        faceStates[i] = shouldRenderFace(camera, vertices, i);
    }
}

bool shouldComputeVertex(unsigned int vertex, bool* faceStates) {
    bool shouldCompute = false;

    for (unsigned int i = 0; i < 6; i++) {
        if (!faceStates[i]) {
            continue;
        }

        if (
            (FACE_VERTICES[(i * 4) + 0] == vertex) ||
            (FACE_VERTICES[(i * 4) + 1] == vertex) ||
            (FACE_VERTICES[(i * 4) + 2] == vertex) ||
            (FACE_VERTICES[(i * 4) + 3] == vertex)
        ) {
            shouldCompute = true;
        }
    }

    return shouldCompute;
}

void drawDisplayLine(DisplayCoords a, DisplayCoords b, color_t colour) {
    if (!a.render || !b.render) {
        return;
    }

    dline(a.x, VIEWPORT_HEIGHT - a.y, b.x, VIEWPORT_HEIGHT - b.y, colour);
}

void addBlockFace(DisplayBlockFaces* faces, DisplayBlockFace face) {
    faces->faces = realloc(faces->faces, ++faces->count * sizeof(face));
    faces->faces[faces->count - 1] = face;
}

void sortBlockFaces(DisplayBlockFaces* faces) {
    for (unsigned int i = 1; i < faces->count; i++) {
        DisplayBlockFace element = faces->faces[i];
        int j = i - 1;

        while (j >= 0 && faces->faces[j].z < element.z) {
            faces->faces[j + 1] = faces->faces[j];
            j--;
        }

        faces->faces[j + 1] = element;
    }
}

void renderBlockFace(DisplayBlockFace face, color_t colour) {
    unsigned int renderLines = MIN(ABS(face.vertices[2].y - face.vertices[0].y) / 2, 20);

    for (double i = 0; i < renderLines; i++) {
        double t = i / renderLines;
        int ax = LERP(face.vertices[0].x, face.vertices[3].x, t);
        int ay = LERP(face.vertices[0].y, face.vertices[3].y, t);
        int bx = LERP(face.vertices[1].x, face.vertices[2].x, t);
        int by = LERP(face.vertices[1].y, face.vertices[2].y, t);

        dline(ax, VIEWPORT_HEIGHT - ay, bx, VIEWPORT_HEIGHT - by, colour);
        dline(ax, VIEWPORT_HEIGHT - ay + 1, bx, VIEWPORT_HEIGHT - by + 1, colour);
    }

    drawDisplayLine(face.vertices[0], face.vertices[1], C_BLACK);
    drawDisplayLine(face.vertices[1], face.vertices[2], C_BLACK);
    drawDisplayLine(face.vertices[2], face.vertices[3], C_BLACK);
    drawDisplayLine(face.vertices[3], face.vertices[0], C_BLACK);
}

void camera_render(Camera camera, World world) {
    #ifdef FLAG_PROFILING
    profiling_start(PROFILING_RENDER_TIME);
    #endif

    DisplayBlockFaces faces = {.count = 0, .faces = malloc(0)};

    for (unsigned int i = 0; i < world.changedBlockCount; i++) {
        Block block = world.changedBlocks[i];
        CartesianVector* vertices = world_getBlockVertices(block);
        DisplayCoords pixelsToSet[8];
        bool faceStates[6];
        unsigned int zSum = 0;
        unsigned int zTotal = 0;

        setRenderFaceStates(faceStates, camera, vertices);

        for (unsigned int face = 0; face < 6; face++) {
            if (adjacentBlockIsFilled(world, block, face)) {
                faceStates[face] = false;
            }
        }

        for (unsigned int vertex = 0; vertex < 8; vertex++) {
            pixelsToSet[vertex] = (DisplayCoords) {0, 0, false};

            if (!shouldComputeVertex(vertex, faceStates)) {
                continue;
            }

            #ifdef FLAG_PROFILING
            profiling_start(PROFILING_WORLD_TO_CAMERA);
            #endif

            CartesianVector relativePoint = camera_worldSpaceToCameraSpace(vertices[vertex], camera.position, camera.heading);

            #ifdef FLAG_PROFILING
            profiling_stop(PROFILING_WORLD_TO_CAMERA);
            #endif

            if (relativePoint.x < 0) {
                continue; // Don't render when behind camera
            }

            zSum += relativePoint.x;
            zTotal++;

            #ifdef FLAG_PROFILING
            profiling_start(PROFILING_ORTH_TO_PERSP);
            #endif

            DisplayCoords pixelToSet = camera_orthToPersp(relativePoint.z, relativePoint.y, relativePoint.x, camera.fov);

            #ifdef FLAG_PROFILING
            profiling_stop(PROFILING_ORTH_TO_PERSP);
            #endif

            pixelsToSet[vertex] = pixelToSet;
        }

        #ifdef FLAG_PROFILING
        profiling_start(PROFILING_FIND_EDGES);
        #endif

        for (unsigned int face = 0; face < sizeof(faceStates) / sizeof(faceStates[0]); face++) {
            if (!faceStates[face]) {
                continue;
            }

            DisplayBlockFace faceToAdd = {.z = zSum / zTotal};

            for (unsigned int i = 0; i < 4; i++) {
                faceToAdd.vertices[i] = pixelsToSet[FACE_VERTICES[(face * 4) + i]];
            }

            addBlockFace(&faces, faceToAdd);
        }

        #ifdef FLAG_PROFILING
        profiling_stop(PROFILING_FIND_EDGES);
        #endif

        free(vertices);
    }

    sortBlockFaces(&faces);

    #ifdef FLAG_PROFILING
    profiling_start(PROFILING_DRAW_FACES);
    #endif

    for (unsigned int i = 0; i < faces.count; i++) {
        renderBlockFace(faces.faces[i], C_LIGHT);
    }

    #ifdef FLAG_PROFILING
    profiling_stop(PROFILING_DRAW_FACES);
    #endif

    free(faces.faces);

    #ifdef FLAG_PROFILING
    profiling_stop(PROFILING_RENDER_TIME);
    #endif
}