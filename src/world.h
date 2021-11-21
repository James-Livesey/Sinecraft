#ifndef WORLD_H_
#define WORLD_H_

#include "coords.h"

enum {
    BLOCK_TYPE_AIR = 0,
    BLOCK_TYPE_STONE = 1
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
Block world_getBlock(World world, CartesianVector position);

#endif