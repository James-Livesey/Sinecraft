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
        return (DisplayCoords) {VIEWPORT_WIDTH / 2, VIEWPORT_HEIGHT / 2, true};
    }

    return (DisplayCoords) {
        orthToPersp2d(x, distance, fov) * VIEWPORT_WIDTH,
        orthToPersp2d(y, distance, verticalFov) * VIEWPORT_HEIGHT,
        true
    };
}

CartesianVector camera_worldSpaceToCameraSpace(CartesianVector vector, CartesianVector cameraPosition, PolarVector cameraHeading) {
    cameraHeading.ariz += 180;

    vector = coords_addCartesian(vector, coords_scaleCartesian(cameraPosition, -1));

    CartesianVector pointMultiply = {1, 1, 1};
    PolarVector pointRotate = {0, 0, 0};

    if (vector.x < 0 && vector.z < 0) {
        pointRotate.ariz += 180;
    } else if (vector.x < 0) {
        pointMultiply.x = -1;
        pointMultiply.z = -1;
    }

    return coords_multiplyCartesian(
        coords_rotateCartesian(vector, coords_addPolar(
            coords_scalePolar(cameraHeading, -1),
            pointRotate
        )),
        pointMultiply
    );
}

LayeredDisplayCoords getBlockVertex(CartesianVector* vertices, unsigned int vertex, Camera camera, LayeredDisplayCoords* cachedDisplayVertices) {
    if (cachedDisplayVertices[vertex].render) {
        return cachedDisplayVertices[vertex];
    }

    #ifdef FLAG_PROFILING
    profiling_start(PROFILING_WORLD_TO_CAMERA);
    #endif

    CartesianVector relativePoint = camera_worldSpaceToCameraSpace(vertices[vertex], camera.position, camera.heading);

    #ifdef FLAG_PROFILING
    profiling_stop(PROFILING_WORLD_TO_CAMERA);
    #endif

    #ifdef FLAG_PROFILING
    profiling_start(PROFILING_ORTH_TO_PERSP);
    #endif

    DisplayCoords vertexCoords = camera_orthToPersp(relativePoint.z, relativePoint.y, relativePoint.x, camera.fov);

    #ifdef FLAG_PROFILING
    profiling_stop(PROFILING_ORTH_TO_PERSP);
    #endif

    cachedDisplayVertices[vertex] = vertexCoords;

    return {vertexCoords.x, vertexCoords.y, relativePoint.x, true};
}

DisplayBlock camera_buildDisplayBlock(Camera camera, Block block) {
    CartesianVector relativePosition = coords_addCartesian(block.position, coords_scaleCartesian(camera.position, -1));
    DisplayBlock displayBlock = {.block = block, .render = true};
    LayeredDisplayCoords nonRenderedCoords = {0, 0, false};
    DisplayFace nonRenderedFace = {.zIndex = 0, .render = false};
    LayeredDisplayCoords cachedDisplayVertices[8];

    for (unsigned int i = 0; i < 4; i++) {
        nonRenderedFace.vertices[i] = nonRenderedCoords;
    }

    for (unsigned int i = 0; i < 8; i++) {
        cachedDisplayVertices[i] = nonRenderedCoords;
    }

    CartesianVector* blockVertices = world_getBlockVertices(block);

    for (unsigned int face = 0; face < 6; face++) {
        if (
            (face == Face.NX && relativePosition.x > -0.5) ||
            (face == Face.PX && relativePosition.x < 0.5) ||
            (face == Face.NY && relativePosition.y > -0.5) ||
            (face == Face.PY && relativePosition.y < 0.5) ||
            (face == Face.NZ && relativePosition.z > -0.5) ||
            (face == Face.PZ && relativePosition.z < 0.5)
        ) {
            displayBlock.faces[face] = nonRenderedFace;

            continue; // Cull backface rendering
        }

        displayBlock.faces[face].render = true;
        displayBlock.faces[face].zIndex = 0;

        for (unsigned int i = 0; i < 4; i++) {
            LayeredDisplayCoords vertex = getBlockVertex(blockVertices, FACE_VERTICES[(face * 4) + i], camera, cachedDisplayVertices);

            displayBlock.faces[face].vertices[i] = vertex;
            displayBlock.faces[face].zIndex += vertex.z;
        }

        displayBlock.faces[face].zIndex /= 4;
    }

    free(blockVertices);

    return displayBlock;
}

void camera_moveInAriz(Camera* camera, double distance, double ariz) {
    camera->position = coords_addCartesian(camera->position, coords_fromPolar((PolarVector) {
        distance,
        90,
        ariz
    }));
}

void drawDisplayLine(DisplayCoords a, DisplayCoords b, color_t colour) {
    if (!a.render || !b.render) {
        return;
    }

    dline(a.x, VIEWPORT_HEIGHT - a.y, b.x, VIEWPORT_HEIGHT - b.y, colour);
}

// TODO: Replace wireframing with filled-in faces
void camera_render(Camera camera, World world) {
    #ifdef FLAG_PROFILING
    profiling_start(PROFILING_RENDER_TIME);
    #endif

    for (unsigned int i = 0; i < world.changedBlockCount; i++) {
        CartesianVector* vertices = world_getBlockVertices(world.changedBlocks[i]);
        DisplayCoords pixelsToSet[8];

        for (unsigned int j = 0; j < 8; j++) {
            pixelsToSet[j] = (DisplayCoords) {0, 0, false};

            #ifdef FLAG_PROFILING
            profiling_start(PROFILING_WORLD_TO_CAMERA);
            #endif

            CartesianVector relativePoint = camera_worldSpaceToCameraSpace(vertices[j], camera.position, camera.heading);

            #ifdef FLAG_PROFILING
            profiling_stop(PROFILING_WORLD_TO_CAMERA);
            #endif

            if (relativePoint.x < 0) {
                continue; // Don't render when behind camera
            }

            #ifdef FLAG_PROFILING
            profiling_start(PROFILING_ORTH_TO_PERSP);
            #endif

            DisplayCoords pixelToSet = camera_orthToPersp(relativePoint.z, relativePoint.y, relativePoint.x, camera.fov);

            #ifdef FLAG_PROFILING
            profiling_stop(PROFILING_ORTH_TO_PERSP);
            #endif

            pixelsToSet[j] = pixelToSet;

            // dpixel(pixelToSet.x, VIEWPORT_HEIGHT - pixelToSet.y, C_BLACK);
        }

        #ifdef FLAG_PROFILING
        profiling_start(PROFILING_DRAW_EDGES);
        #endif

        drawDisplayLine(pixelsToSet[0], pixelsToSet[1], C_BLACK);
        drawDisplayLine(pixelsToSet[0], pixelsToSet[2], C_BLACK);
        drawDisplayLine(pixelsToSet[1], pixelsToSet[3], C_BLACK);
        drawDisplayLine(pixelsToSet[1], pixelsToSet[5], C_BLACK);
        drawDisplayLine(pixelsToSet[2], pixelsToSet[3], C_BLACK);
        drawDisplayLine(pixelsToSet[3], pixelsToSet[7], C_BLACK);
        drawDisplayLine(pixelsToSet[4], pixelsToSet[0], C_BLACK);
        drawDisplayLine(pixelsToSet[4], pixelsToSet[6], C_BLACK);
        drawDisplayLine(pixelsToSet[5], pixelsToSet[4], C_BLACK);
        drawDisplayLine(pixelsToSet[5], pixelsToSet[7], C_BLACK);
        drawDisplayLine(pixelsToSet[6], pixelsToSet[2], C_BLACK);
        drawDisplayLine(pixelsToSet[7], pixelsToSet[6], C_BLACK);

        #ifdef FLAG_PROFILING
        profiling_stop(PROFILING_DRAW_EDGES);
        #endif

        free(vertices);
    }

    #ifdef FLAG_PROFILING
    profiling_stop(PROFILING_RENDER_TIME);
    #endif
}