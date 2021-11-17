#include <math.h>
#include <gint/display.h>

#include "camera.h"
#include "common.h"
#include "coords.h"
#include "objects.h"

struct dithering {
    unsigned char maxValue;
    color_t a;
    color_t b;
};

struct dithering ditheringLut[7] = {
    {0, C_BLACK, C_BLACK},
    {43, C_BLACK, C_DARK},
    {85, C_DARK, C_DARK},
    {128, C_DARK, C_LIGHT},
    {170, C_LIGHT, C_LIGHT},
    {213, C_LIGHT, C_WHITE},
    {255, C_WHITE, C_WHITE}
};

Camera camera_default() {
    return (Camera) {
        .position = (CartesianVector) {0, 0, 0},
        .heading = (PolarVector) {0, PI / 2, -PI / 4},
        .fov = 60,
        .maxRenderDistance = 3
    };
}

char camera_shootRay(Camera camera, PolarVector direction) {
    for (double r = 0; r < camera.maxRenderDistance; r += 0.5) {
        direction.r = r;

        CartesianVector clippingPoint = coords_fromPolar(direction);

        clippingPoint.x += camera.position.x;
        clippingPoint.y += camera.position.y;
        clippingPoint.z += camera.position.z;

        if (objects_hasClippedObject(clippingPoint)) {
            return objects_getClippedObjectColor(clippingPoint);
            // return LERP(objects_getClippedObjectColor(clippingPoint), 0xFF, r / camera.maxRenderDistance);
        }
    }

    return SKY_COLOUR;
}

double fovRadians(Camera camera) {
    return (camera.fov / 180) * PI;
}

color_t camera_ditherPixel(int x, int y, unsigned char value) {
    for (unsigned int i = 0; i < 7; i++) {
        if (value <= ditheringLut[i].maxValue) {
            return ((x % 2) ^ (y % 2)) == 1 ? ditheringLut[i].b : ditheringLut[i].a;
        }
    }

    return C_WHITE;
    // return ((x % 2) ^ (y % 2)) == 1 ? C_WHITE : C_LIGHT;
}

void camera_render(Camera camera) {
    double distanceFromFrustrum = (FRAMEBUFFER_ASPECT_RATIO / 2) * (1 / tan(fovRadians(camera) / 2));
    CartesianVector topLeft = {-FRAMEBUFFER_ASPECT_RATIO / 2, 0.5, 0};
    CartesianVector bottomRight = {FRAMEBUFFER_ASPECT_RATIO / 2, -0.5, 0};

    int xMultiplier = 128 / FRAMEBUFFER_WIDTH;
    int yMultiplier = 64 / FRAMEBUFFER_HEIGHT;

    for (unsigned int y = 0; y < FRAMEBUFFER_HEIGHT; y++) {
        for (unsigned int x = 0; x < FRAMEBUFFER_WIDTH; x++) {
            PolarVector vectorToFrustrum = coords_fromCartesian((CartesianVector) {
                distanceFromFrustrum,
                LERP(topLeft.y, bottomRight.y, y / FRAMEBUFFER_HEIGHT),
                LERP(topLeft.x, bottomRight.x, x / FRAMEBUFFER_WIDTH)
            }); // (0, 0, 0) is camera centre at this instance

            vectorToFrustrum.incl -= camera.heading.incl - (PI / 2);
            vectorToFrustrum.ariz -= camera.heading.ariz;

            if (vectorToFrustrum.incl < 0) {
                vectorToFrustrum.incl += PI;
            }

            drect(
                x * xMultiplier, y * yMultiplier,
                (x + 1) * xMultiplier, (y + 1) * yMultiplier,
                camera_ditherPixel(x, y, camera_shootRay(camera, vectorToFrustrum))
            );
        }
    }
}