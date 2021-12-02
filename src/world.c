#include <stdlib.h>

#include "world.h"
#include "coords.h"
#include "textures.h"

const int BLOCK_INFO[] = {
    // Block type, texture 0-5
    BLOCK_TYPE_WOOD, TEXTURE_WOOD, TEXTURE_WOOD, TEXTURE_WOOD_Y, TEXTURE_WOOD_Y, TEXTURE_WOOD, TEXTURE_WOOD,
    BLOCK_TYPE_LEAVES, TEXTURE_LEAVES, TEXTURE_LEAVES, TEXTURE_LEAVES, TEXTURE_LEAVES, TEXTURE_LEAVES, TEXTURE_LEAVES
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