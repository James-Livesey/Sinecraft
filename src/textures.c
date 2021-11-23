#include "textures.h"

Texture textures[TEXTURE_COUNT];

Texture textures_default() {
    Texture texture = {.linesToRender = 0};

    for (unsigned int i = 0; i < LINES_PER_TEXTURE; i++) {
        texture.lines[i] = (TextureLine) {0, 0, 0, 0};
    }

    return texture;
}

void textures_init() {
    textures[TEXTURE_DEFAULT] = textures_default();

    textures[TEXTURE_GRASS] = textures[TEXTURE_DEFAULT];

    textures[TEXTURE_GRASS].lines[0] = (TextureLine) {0, 3, 16, 3};
    textures[TEXTURE_GRASS].linesToRender = 1;

    textures[TEXTURE_WOOD] = textures[TEXTURE_DEFAULT];

    textures[TEXTURE_WOOD].lines[0] = (TextureLine) {2, 0, 2, 16};
    textures[TEXTURE_WOOD].lines[1] = (TextureLine) {5, 0, 5, 5};
    textures[TEXTURE_WOOD].lines[2] = (TextureLine) {5, 8, 5, 16};
    textures[TEXTURE_WOOD].lines[3] = (TextureLine) {8, 0, 8, 16};
    textures[TEXTURE_WOOD].lines[4] = (TextureLine) {11, 0, 11, 11};
    textures[TEXTURE_WOOD].lines[5] = (TextureLine) {11, 14, 11, 16};
    textures[TEXTURE_WOOD].lines[6] = (TextureLine) {14, 0, 14, 16};
    textures[TEXTURE_WOOD].linesToRender = 7;

    textures[TEXTURE_WOOD].lines[0] = (TextureLine) {0, 0, 6, 12};
    textures[TEXTURE_WOOD].lines[1] = (TextureLine) {3, 5, 16, 2};
    textures[TEXTURE_WOOD].lines[2] = (TextureLine) {10, 3, 13, 13};
    textures[TEXTURE_WOOD].lines[3] = (TextureLine) {0, 11, 16, 14};
    textures[TEXTURE_WOOD].lines[4] = (TextureLine) {8, 13, 5, 16};
    textures[TEXTURE_WOOD].linesToRender = 7;
}