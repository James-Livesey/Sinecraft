#ifndef TEXTURES_H_
#define TEXTURES_H_

#define LINES_PER_TEXTURE 8
#define TEXTURE_COUNT 1

enum {
    TEXTURE_TEST
};

typedef struct {
    int x1;
    int y1;
    int x2;
    int y2;
} TextureLine;

typedef struct {
    TextureLine lines[LINES_PER_TEXTURE];
    unsigned int linesToRender;
} Texture;

extern Texture textures[TEXTURE_COUNT];

Texture textures_default();
void textures_init();

#endif