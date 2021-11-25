#include <stdlib.h>
#include <math.h>
#include <gint/display.h>
#include <gint/display-fx.h>

#include "flags.h"
#include "camera.h"
#include "common.h"
#include "coords.h"
#include "textures.h"
#include "world.h"
#include "profiling.h"

extern bopti_image_t img_crosshair;

const int VIEWPORT_WIDTH = 128;
const int VIEWPORT_HEIGHT = 64;
const int VIEWPORT_CENTRE_X = VIEWPORT_WIDTH / 2;
const int VIEWPORT_CENTRE_Y = VIEWPORT_HEIGHT / 2;

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

DisplayBlockFace lastSelectedFace;
bool blockCurrentlySelected = false;
bool blockIsGround = false;

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
        verticalFov = 2 * common_atan(VIEWPORT_CENTRE_Y / (VIEWPORT_CENTRE_X / common_tan(fov / 2)));
    }

    if (distance == 0) {
        return (DisplayCoords) {VIEWPORT_CENTRE_X, VIEWPORT_CENTRE_Y, false};
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

// See https://github.com/michaelerule/Uno9341TFT/blob/e4326d0e73ded32ef3aa4b3806ed5bf54886d2c9/examples/self_contained_Uno9341TFT_graphics_test_fast_commands/Arduino_TFTLCD.cpp#L837 for implementation
// This procedure is licenced under the BSD License
void drawDisplayTriangle(DisplayCoords a, DisplayCoords b, DisplayCoords c, color_t colour) {
    int ia;
    int ib;
    int y;

    if (a.y > b.y) {
        SWAPCOORDS(a, b);
    }

    if (b.y > c.y) {
        SWAPCOORDS(b, c);
    }

    if (a.y > b.y) {
        SWAPCOORDS(a, b);
    }

    if (a.y == c.y) {
        return;
    }

    int dxAB = b.x - a.x;
    int dyAB = b.y - a.y;
    int dxAC = c.x - a.x;
    int dyAC = c.y - a.y;
    int dxBC = c.x - b.x;
    int dyBC = c.y - b.y;

    int sideA = 0;
    int sideB = 0;
    int last = b.y == c.y ? b.y : b.y - 1;

    sideA += dxAB;
    sideB += dxAC;

    for (y = a.y + 1; y <= last; y++) {
        ia = a.x + (sideA / dyAB);
        ib = a.x + (sideB / dyAC);
        sideA += dxAB;
        sideB += dxAC;

        if (ia > ib) {
            SWAPINT(ia, ib);
        }

        dline(ia, VIEWPORT_HEIGHT - y, ib, VIEWPORT_HEIGHT - y, colour);
    }

    sideA = dxBC * (y - b.y);
    sideB = dxAC * (y - a.y);

    for (; y < c.y; y++) {
        ia = b.x + (sideA / dyBC);
        ib = a.x + (sideB / dyAC);
        sideA += dxBC;
        sideB += dxAC;

        if (ia > ib) {
            SWAPINT(ia, ib);
        }

        dline(ia, VIEWPORT_HEIGHT - y, ib, VIEWPORT_HEIGHT - y, colour);
    }
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

DisplayCoords findPointInFace(DisplayBlockFace face, double xt, double yt) {
    int minX = LERP((double)face.vertices[0].x, (double)face.vertices[3].x, yt / 16);
    int maxX = LERP((double)face.vertices[1].x, (double)face.vertices[2].x, yt / 16);
    int minY = LERP((double)face.vertices[0].y, (double)face.vertices[1].y, xt / 16);
    int maxY = LERP((double)face.vertices[3].y, (double)face.vertices[2].y, xt / 16);

    return (DisplayCoords) {LERP(minX, maxX, xt / 16), LERP(minY, maxY, yt / 16), true};
}

bool inBounds(int a, int b, int value) {
    if (a < b) {
        return a <= value && value <= b;
    }

    return b <= value && value <= a;
}

void renderBlockFace(DisplayBlockFace face) {
    for (unsigned int i = 0; i < 4; i++) {
        if (!face.vertices[i].render) {
            return;
        }
    }

    if (
        face.z <= MAX_FACE_SELECT_DISTANCE &&
        inBounds(face.vertices[0].x, face.vertices[1].x, VIEWPORT_CENTRE_X) &&
        inBounds(face.vertices[3].x, face.vertices[2].x, VIEWPORT_CENTRE_X) &&
        inBounds(face.vertices[0].y, face.vertices[3].y, VIEWPORT_CENTRE_Y) &&
        inBounds(face.vertices[1].y, face.vertices[2].y, VIEWPORT_CENTRE_Y)
    ) {
        lastSelectedFace = face;
        blockCurrentlySelected = true;
        blockIsGround = false;
    }

    drawDisplayTriangle(face.vertices[0], face.vertices[1], face.vertices[2], C_WHITE);
    drawDisplayTriangle(face.vertices[0], face.vertices[2], face.vertices[3], C_WHITE);

    drawDisplayLine(face.vertices[0], face.vertices[1], C_BLACK);
    drawDisplayLine(face.vertices[1], face.vertices[2], C_BLACK);
    drawDisplayLine(face.vertices[2], face.vertices[3], C_BLACK);
    drawDisplayLine(face.vertices[3], face.vertices[0], C_BLACK);

    if (face.z > MAX_TEXTURE_RENDER_DISTANCE) {
        return;
    }

    if (ABS(face.vertices[1].x - face.vertices[0].x) < 8 || ABS(face.vertices[3].y - face.vertices[0].y) < 8) {
        return;
    }

    for (unsigned int i = 0; i < textures[face.texture].linesToRender; i++) {
        TextureLine line = textures[face.texture].lines[i];

        drawDisplayLine(findPointInFace(face, line.x1, line.y1), findPointInFace(face, line.x2, line.y2), C_BLACK);
    }
}

void renderBlockFaceSelected(DisplayBlockFace face) {
    for (unsigned int i = 0; i < 4; i++) {
        if (!face.vertices[i].render) {
            return;
        }
    }

    drawDisplayTriangle(face.vertices[0], face.vertices[1], face.vertices[2], C_INVERT);
    drawDisplayTriangle(face.vertices[0], face.vertices[2], face.vertices[3], C_INVERT);

    drawDisplayLine(face.vertices[0], face.vertices[2], C_INVERT);
}

void selectFaceTowardsGround(Camera camera) {
    PolarVector heading = camera.heading;

    heading.ariz += 180;
    heading.incl += 90;
    heading.incl *= -1;

    Block block;
    bool foundBlock = false;

    for (double r = 0.5; r <= MAX_FACE_SELECT_DISTANCE; r++) {
        heading.r = r;

        CartesianVector offset = coords_fromPolar(heading);
        CartesianVector vector = coords_addCartesian(camera.position, offset);

        if (vector.y <= 0) {
            if (round(offset.x) == 0 && round(offset.z) == 0 && camera.position.y < 2) {
                return;
            }

            block = (Block) {
                .position = (CartesianVector) {round(vector.x), -1, round(vector.z)},
                .type = BLOCK_TYPE_GRASS
            };

            foundBlock = true;

            break;
        }
    }

    if (!foundBlock) {
        return;
    }

    CartesianVector* vertices = world_getBlockVertices(block);
    DisplayBlockFace faceToSelect = {
        .block = &block,
        .face = FACE_PY,
        .z = 0,
        .texture = BLOCK_TYPE_GRASS
    };

    for (unsigned int i = 0; i < 4; i++) {
        CartesianVector relativePoint = camera_worldSpaceToCameraSpace(vertices[FACE_VERTICES[(FACE_PY * 4) + i]], camera.position, camera.heading);

        faceToSelect.vertices[i] = camera_orthToPersp(relativePoint.z, relativePoint.y, relativePoint.x, camera.fov);
    }

    lastSelectedFace = faceToSelect;
    blockCurrentlySelected = true;
    blockIsGround = true;

    renderBlockFaceSelected(faceToSelect);

    free(vertices);
}

void camera_render(Camera camera, World world) {
    #ifdef FLAG_PROFILING
    profiling_start(PROFILING_RENDER_TIME);
    #endif

    DisplayBlockFaces faces = {.count = 0, .faces = malloc(0)};

    blockCurrentlySelected = false;

    for (unsigned int i = 0; i < world.changedBlockCount; i++) {
        Block* block = &world.changedBlocks[i];

        if (block->type == BLOCK_TYPE_AIR) {
            continue; // Don't render air
        }

        CartesianVector* vertices = world_getBlockVertices(*block);
        DisplayCoords pixelsToSet[8];
        int vertexZValues[8];
        bool faceStates[6];

        setRenderFaceStates(faceStates, camera, vertices);

        for (unsigned int face = 0; face < 6; face++) {
            if (adjacentBlockIsFilled(world, *block, face)) {
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

            vertexZValues[vertex] = relativePoint.x;

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

            pixelsToSet[vertex] = pixelToSet;
        }

        #ifdef FLAG_PROFILING
        profiling_start(PROFILING_FIND_EDGES);
        #endif

        for (unsigned int face = 0; face < 6; face++) {
            if (!faceStates[face]) {
                continue;
            }

            double zSum = 0;

            for (unsigned int i = 0; i < 4; i++) {
                zSum += vertexZValues[FACE_VERTICES[(face * 4) + i]];
            }

            DisplayBlockFace faceToAdd = {
                .block = block,
                .face = face,
                .z = zSum / 4,
                .texture = world_getBlockTexture(block->type, face)
            };

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
        DisplayBlockFace face = faces.faces[i];

        renderBlockFace(face);
    }

    if (blockCurrentlySelected) {
        renderBlockFaceSelected(lastSelectedFace);
    } else {
        selectFaceTowardsGround(camera);
    }

    dimage((VIEWPORT_WIDTH - img_crosshair.width) / 2, (VIEWPORT_HEIGHT - img_crosshair.height) / 2, &img_crosshair);

    #ifdef FLAG_PROFILING
    profiling_stop(PROFILING_DRAW_FACES);
    #endif

    if (faces.count > 0) {
        free(faces.faces);
    }

    #ifdef FLAG_PROFILING
    profiling_stop(PROFILING_RENDER_TIME);
    #endif
}

void camera_destroySelectedBlock(World* world) {
    if (!blockCurrentlySelected || blockIsGround) {
        return;
    }

    blockCurrentlySelected = false;

    Block blockToDestroy = *lastSelectedFace.block;

    blockToDestroy.type = BLOCK_TYPE_AIR;

    world_setBlock(world, blockToDestroy);
}

void camera_placeBlockOnFace(World* world, Camera camera) {
    if (!blockCurrentlySelected) {
        return;
    }

    blockCurrentlySelected = false;

    CartesianVector position = getAdjacentBlockPosition(*lastSelectedFace.block, lastSelectedFace.face);

    if (position.x < 0 || position.y < 0 || position.z < 0) {
        return;
    }

    if (
        position.x > camera.position.x - 0.5 && position.x < camera.position.x + 0.5 &&
        position.y > camera.position.y - 0.5 && position.y < camera.position.y + 1.5 &&
        position.z > camera.position.z - 0.5 && position.z < camera.position.z + 0.5
    ) {
        return;
    }

    Block blockToPlace = (Block) {
        .position = position,
        .type = BLOCK_TYPE_STONE
    };

    world_setBlock(world, blockToPlace);
}