#ifndef CAMERA_H_
#define CAMERA_H_

#include <gint/display-fx.h>

#include "coords.h"

#define FRAMEBUFFER_WIDTH 32
#define FRAMEBUFFER_HEIGHT 16
#define FRAMEBUFFER_ASPECT_RATIO (FRAMEBUFFER_WIDTH / FRAMEBUFFER_HEIGHT)
#define SKY_COLOUR 213

typedef struct {
    CartesianVector position;
    PolarVector heading;
    int fov;
    int maxRenderDistance;
} Camera;

Camera camera_default();
char camera_shootRay(Camera camera, PolarVector direction);
color_t camera_ditherPixel(int x, int y, unsigned char value);
void camera_render(Camera camera);

#endif