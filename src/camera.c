#include <stdlib.h>
#include <math.h>
#include <gint/display.h>
#include <gint/display-fx.h>

#include "camera.h"
#include "common.h"
#include "coords.h"
#include "world.h"

const int VIEWPORT_WIDTH = 128;
const int VIEWPORT_HEIGHT = 64;

Camera camera_default() {
    return (Camera) {
        .position = coords_defaultCartesian(),
        .heading = coords_defaultPolar(),
        .fov = 60
    };
}

double orthToPersp2d(double value, double distance, double fovRad) {
    float v1 = distance * tan(fovRad / 2);
    float v0 = -v1;
    float t = INVLERP(v0, v1, value);

    return LERP(0, 1, t);
}

DisplayCoords camera_orthToPersp(double x, double y, double distance, double fovDeg) {
    double fovRad = (fovDeg / 180) * PI;
    double verticalFovRad = 2 * atan((VIEWPORT_HEIGHT / 2) / ((VIEWPORT_WIDTH / 2) / atan(fovRad / 2)));

    if (distance == 0) {
        return (DisplayCoords) {VIEWPORT_WIDTH / 2, VIEWPORT_HEIGHT / 2, true};
    }

    return (DisplayCoords) {
        orthToPersp2d(x, distance, fovRad) * VIEWPORT_WIDTH,
        orthToPersp2d(y, distance, verticalFovRad) * VIEWPORT_HEIGHT,
        true
    };
}

CartesianVector camera_worldSpaceToCameraSpace(CartesianVector vector, CartesianVector cameraPosition, PolarVector cameraHeading) {
    cameraHeading.ariz += PI;

    vector = coords_addCartesian(vector, coords_scaleCartesian(cameraPosition, -1));

    CartesianVector pointMultiply = {1, 1, 1};
    PolarVector pointRotate = {0, 0, 0};

    if (vector.x < 0 && vector.z < 0) {
        pointRotate.ariz += PI;
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

void camera_moveInAriz(Camera* camera, double distance, double ariz) {
    camera->position = coords_addCartesian(camera->position, coords_fromPolar((PolarVector) {
        distance,
        PI / 2,
        ariz
    }));
}

void drawDisplayLine(DisplayCoords a, DisplayCoords b, color_t colour) {
    if (!a.render || !b.render) {
        return;
    }

    dline(a.x, VIEWPORT_HEIGHT - a.y, b.x, VIEWPORT_HEIGHT - b.y, colour);
}

void camera_render(Camera camera, World world) {
    for (unsigned int i = 0; i < world.changedBlockCount; i++) {
        CartesianVector* vertices = world_getBlockVertices(world.changedBlocks[i]);
        DisplayCoords pixelsToSet[8];

        for (unsigned int j = 0; j < 8; j++) {
            pixelsToSet[j] = (DisplayCoords) {0, 0, false};

            CartesianVector relativePoint = camera_worldSpaceToCameraSpace(vertices[j], camera.position, camera.heading);

            if (relativePoint.x < 0) {
                continue; // Don't render when behind camera
            }

            DisplayCoords pixelToSet = camera_orthToPersp(relativePoint.z, relativePoint.y, relativePoint.x, camera.fov);

            pixelsToSet[j] = pixelToSet;

            // dpixel(pixelToSet.x, VIEWPORT_HEIGHT - pixelToSet.y, C_BLACK);
        }

        drawDisplayLine(pixelsToSet[0], pixelsToSet[1], C_BLACK);
        drawDisplayLine(pixelsToSet[0], pixelsToSet[2], C_BLACK);
        drawDisplayLine(pixelsToSet[0], pixelsToSet[4], C_BLACK);
        drawDisplayLine(pixelsToSet[1], pixelsToSet[3], C_BLACK);
        drawDisplayLine(pixelsToSet[1], pixelsToSet[5], C_BLACK);
        drawDisplayLine(pixelsToSet[2], pixelsToSet[3], C_BLACK);
        drawDisplayLine(pixelsToSet[2], pixelsToSet[6], C_BLACK);
        drawDisplayLine(pixelsToSet[3], pixelsToSet[7], C_BLACK);
        drawDisplayLine(pixelsToSet[4], pixelsToSet[5], C_BLACK);
        drawDisplayLine(pixelsToSet[4], pixelsToSet[6], C_BLACK);
        drawDisplayLine(pixelsToSet[5], pixelsToSet[7], C_BLACK);
        drawDisplayLine(pixelsToSet[6], pixelsToSet[7], C_BLACK);

        free(vertices);
    }
}