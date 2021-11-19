#include <stdlib.h>

#include "world.h"
#include "coords.h"

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
    world->changedBlocks = realloc(world->changedBlocks, ++world->changedBlockCount * sizeof(Block));
    world->changedBlocks[world->changedBlockCount - 1] = block;
}