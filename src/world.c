#include <stdlib.h>
#include <string.h>
#include <gint/cpu.h>
#include <gint/bfile.h>

#include "world.h"
#include "coords.h"
#include "textures.h"

const int BLOCK_INFO[] = {
    // Block type, texture 0-5
    BLOCK_TYPE_GRASS, TEXTURE_GRASS, TEXTURE_GRASS, TEXTURE_DEFAULT, TEXTURE_DEFAULT, TEXTURE_GRASS, TEXTURE_GRASS,
    BLOCK_TYPE_WOOD, TEXTURE_WOOD, TEXTURE_WOOD, TEXTURE_WOOD_Y, TEXTURE_WOOD_Y, TEXTURE_WOOD, TEXTURE_WOOD,
    BLOCK_TYPE_LEAVES, TEXTURE_LEAVES, TEXTURE_LEAVES, TEXTURE_LEAVES, TEXTURE_LEAVES, TEXTURE_LEAVES, TEXTURE_LEAVES,
    BLOCK_TYPE_PLANK, TEXTURE_PLANK, TEXTURE_PLANK, TEXTURE_PLANK, TEXTURE_PLANK, TEXTURE_PLANK, TEXTURE_PLANK,
    BLOCK_TYPE_CRAFTING_TABLE, TEXTURE_CRAFTING_TABLE, TEXTURE_CRAFTING_TABLE, TEXTURE_DEFAULT, TEXTURE_CRAFTING_TABLE_TOP, TEXTURE_CRAFTING_TABLE, TEXTURE_CRAFTING_TABLE
};

CartesianVector* world_getBlockVertices(Block block) {
    CartesianVector* vertices = malloc(8 * sizeof(CartesianVector));

    CartesianVector cornerA = coords_addCartesian(block.position, (CartesianVector) {-0.5, -0.5, -0.5});
    CartesianVector cornerB = coords_addCartesian(block.position, (CartesianVector) {0.5, 0.5, 0.5});

    vertices[0] = cornerA;
    vertices[1] = (CartesianVector) {cornerA.x, cornerA.y, cornerB.z};
    vertices[2] = (CartesianVector) {cornerA.x, cornerB.y, cornerA.z};
    vertices[3] = (CartesianVector) {cornerA.x, cornerB.y, cornerB.z};
    vertices[4] = (CartesianVector) {cornerB.x, cornerA.y, cornerA.z};
    vertices[5] = (CartesianVector) {cornerB.x, cornerA.y, cornerB.z};
    vertices[6] = (CartesianVector) {cornerB.x, cornerB.y, cornerA.z};
    vertices[7] = cornerB;

    return vertices;
}

World world_default() {
    return (World) {
        .changedBlockCount = 0,
        .changedBlocks = malloc(0)
    };
}

unsigned int world_getSize(World world) {
    return sizeof(World) + (world.changedBlockCount * sizeof(Block));
}

void world_addBlock(World* world, Block block) {
    world->changedBlocks = realloc(world->changedBlocks, ++world->changedBlockCount * sizeof(block));
    world->changedBlocks[world->changedBlockCount - 1] = block;
}

void world_setBlock(World* world, Block block) {
    for (unsigned int i = 0; i < world->changedBlockCount; i++) {
        if (coords_equalCartesian(block.position, world->changedBlocks[i].position)) {
            world->changedBlocks[i] = block;

            return;
        }
    }

    world_addBlock(world, block);
}

Block world_getBlock(World world, CartesianVector position) {
    for (unsigned int i = 0; i < world.changedBlockCount; i++) {
        if (coords_equalCartesian(position, world.changedBlocks[i].position)) {
            return world.changedBlocks[i];
        }
    }

    return (Block) {
        .position = position,
        .type = BLOCK_TYPE_AIR
    };
}

int world_getBlockTexture(int blockType, int face) {
    for (unsigned int i = 0; i < sizeof(BLOCK_INFO) / sizeof(BLOCK_INFO[0]); i += 7) {
        if (BLOCK_INFO[i] == blockType) {
            return BLOCK_INFO[i + face + 1];
        }
    }

    return TEXTURE_DEFAULT;
}

int world_save(World world, char* name) {
    cpu_atomic_start();

    int status;
    int size = world_getSize(world);

    uint16_t worldFilePath[30];
    char worldFilePathBuilder[30];

    strcpy(worldFilePathBuilder, WORLD_FILE_PATH_BASE);
    strcat(worldFilePathBuilder, name);
    strcat(worldFilePathBuilder, WORLD_FILE_PATH_EXT);

    for (unsigned int i = 0; i < 30; i++) {
        worldFilePath[i] = worldFilePathBuilder[i];
    }

    BFile_Remove(worldFilePath);
    BFile_Create(worldFilePath, BFile_File, &size);

    int file = BFile_Open(worldFilePath, BFile_WriteOnly);

    if (file < 0) {
        status = file;

        goto saveEnd; // Something went wrong
    }

    status = BFile_Write(file, &world, size);

    BFile_Close(file);

    saveEnd:

    cpu_atomic_end();

    return status;
}