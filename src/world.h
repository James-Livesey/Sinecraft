#ifndef WORLD_H_
#define WORLD_H_

#include "coords.h"

typedef struct {
    CartesianVector position;
} Block;

typedef struct {
    unsigned int changedBlockCount;
    Block* changedBlocks;
} World;

CartesianVector* world_getBlockVertices(Block block);

World world_default();
void world_addBlock(World* world, Block block);

#endif