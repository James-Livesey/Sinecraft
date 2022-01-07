#ifndef WORLD_H_
#define WORLD_H_

#include "coords.h"
#include "inventory.h"

#define MAX_WORLD_NAME_LENGTH 8
#define WORLD_FILE_PATH_BASE "\\\\fls0\\"
#define WORLD_FILE_PATH_EXT ".scw"
#define MAX_WORLD_SIZE 16 * 1024 // 16 KiB - when changing this, update error message too

enum {
    BLOCK_TYPE_AIR = 0,
    BLOCK_TYPE_STONE = 1,
    BLOCK_TYPE_GRASS = 2,
    BLOCK_TYPE_DIRT = 3,
    BLOCK_TYPE_COBBLESTONE = 4,
    BLOCK_TYPE_PLANK = 5,
    BLOCK_TYPE_WOOD = 17,
    BLOCK_TYPE_LEAVES = 18,
    BLOCK_TYPE_CRAFTING_TABLE = 58,
    ITEM_TYPE_WOODEN_AXE = 271,
    ITEM_TYPE_STICK = 280
};

enum {
    WORLD_SAVE_SUCCESS = 0,
    WORLD_SAVE_CANNOT_LOAD = -1,
    WORLD_SAVE_TOO_BIG = -2,
    WORLD_SAVE_NEWER_THAN_EXPECTED = -3
};

typedef struct {
    CartesianVector position;
    unsigned int type;
} Block;

typedef struct {
    unsigned int changedBlockCount;
    Block* changedBlocks;
} World;

typedef struct {
    unsigned int vernum;
    CartesianVector initialCameraPosition;
    PolarVector initialCameraHeading;
    Inventory inventory;
    World world;
} WorldSave;

typedef struct {
    int status;
    WorldSave worldSave;
} WorldSaveStatus;

CartesianVector* world_getBlockVertices(Block block);

World world_default();
void world_addBlock(World* world, Block block);
void world_setBlock(World* world, Block block);
Block world_getBlock(World world, CartesianVector position);

WorldSave world_defaultSave();
unsigned int world_getSaveSize(WorldSave worldSave);

int world_getBlockTexture(int blockType, int face);

WorldSaveStatus world_load(char* name);
int world_save(WorldSave worldSave, char* name);

#endif