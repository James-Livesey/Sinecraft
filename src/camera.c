#include <math.h>
#include <gint/display.h>
#include <gint/display-fx.h>

#include "camera.h"
#include "common.h"
#include "coords.h"

const int VIEWPORT_WIDTH = 128;
const int VIEWPORT_HEIGHT = 64;

const CartesianVector POINTS[] = {
    (CartesianVector) {0, 0, 0},
    (CartesianVector) {0, 0, 1},
    (CartesianVector) {0, 1, 0},
    (CartesianVector) {0, 1, 1},
    (CartesianVector) {1, 0, 0},
    (CartesianVector) {1, 0, 1},
    (CartesianVector) {1, 1, 0},
    (CartesianVector) {1, 1, 1},
    (CartesianVector) {2, 0, 0},
    (CartesianVector) {2, 0, 1},
    (CartesianVector) {2, 1, 0},
    (CartesianVector) {2, 1, 1},
    (CartesianVector) {3, 0, 0},
    (CartesianVector) {3, 0, 1},
    (CartesianVector) {3, 1, 0},
    (CartesianVector) {3, 1, 1},
    (CartesianVector) {1, 1, 0},
    (CartesianVector) {1, 1, 1},
    (CartesianVector) {1, 2, 0},
    (CartesianVector) {1, 2, 1},
    (CartesianVector) {2, 1, 0},
    (CartesianVector) {2, 1, 1},
    (CartesianVector) {2, 2, 0},
    (CartesianVector) {2, 2, 1}
};

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

    if (distance == 0) {
        return (DisplayCoords) {VIEWPORT_WIDTH / 2, VIEWPORT_HEIGHT / 2};
    }

    return (DisplayCoords) {
        orthToPersp2d(x, distance, fovRad) * VIEWPORT_WIDTH,
        orthToPersp2d(y, distance, fovRad) * VIEWPORT_HEIGHT
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

void camera_moveInAriz(Camera *camera, double distance, double ariz) {
    camera->position = coords_addCartesian(camera->position, coords_fromPolar((PolarVector) {
        distance,
        PI / 2,
        ariz
    }));
}

void camera_render(Camera camera) {
    for (unsigned int i = 0; i < sizeof(POINTS) / sizeof(POINTS[0]); i++) {
        CartesianVector relativePoint = camera_worldSpaceToCameraSpace(POINTS[i], camera.position, camera.heading);

        if (relativePoint.x < 0) {
            continue; // Don't render when behind camera
        }

        DisplayCoords pixelToSet = camera_orthToPersp(relativePoint.z, relativePoint.y, relativePoint.x, camera.fov);

        if (pixelToSet.x < 0 || pixelToSet.x > VIEWPORT_WIDTH || pixelToSet.y < 0 || pixelToSet.y > VIEWPORT_WIDTH) {
            continue;
        }

        dpixel(pixelToSet.x, VIEWPORT_HEIGHT - pixelToSet.y, C_BLACK);
    }
}