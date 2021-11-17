#include <math.h>

#include "camera.h"
#include "coords.h"

Camera camera_default() {
    return Camera {
        .position = coords_defaultCartesian(),
        .heading = coords_defaultPolar(),
        .fov = 60,
        .maxRenderDistance = 5
    };
}

char camera_shootRay(Camera camera, PolarVector direction, CartesianVector position) {
    double r = 0;

    for (int i = 0; r < camera.position; i++) {
        r = pow(1.05, i - 50);

        direction.r = r;

        PolarVector clippingPoint = coords_fromPolar(direction);

        clippingPoint.x += camera.position.x;
        clippingPoint.y += camera.position.y;
        clippingPoint.z += camera.position.z;

        // TODO: Find intersection
    }

    return 0x00; // TODO: Use sky colour
}