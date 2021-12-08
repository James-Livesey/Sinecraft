#ifndef WORLD_H_
#define WORLD_H_

#include "coords.h"

enum {
    BLOCK_TYPE_AIR = 0,
    BLOCK_TYPE_STONE = 1,
    BLOCK_TYPE_GRASS = 2,
    BLOCK_TYPE_DIRT = 3,
    BLOCK_TYPE_COBBLESTONE = 4,
    BLOCK_TYPE_PLANK = 5,
    BLOCK_TYPE_WOOD = 17,
    BLOCK_TYPE_LEAVES = 18,
    BLOCK_TYPE_CRAFTING_TABLE = 58
};

typedef struct {
    CartesianVector position;
    unsigned int type;
} Block;

typedef struct {
    unsigned int changedBlockCount;
    Block* changedBlocks;
} World;

CartesianVector* world_getBlockVertices(Block block);

World world_default();
void world_addBlock(World* world, Block block);
void world_setBlock(World* world, Block block);
Block world_getBlock(World world, CartesianVector position);

int world_getBlockTexture(int blockType, int face);

#endif