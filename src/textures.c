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
    textures[TEXTURE_TEST] = textures_default();

    textures[TEXTURE_TEST].lines[0] = (TextureLine) {1, 1, 15, 15};
    textures[TEXTURE_TEST].lines[1] = (TextureLine) {15, 1, 1, 15};
    textures[TEXTURE_TEST].linesToRender = 2;
}