#include <stdlib.h>
#include <string.h>
#include <gint/cpu.h>
#include <gint/bfile.h>

#include "world.h"
#include "flags.h"
#include "serial.h"
#include "coords.h"
#include "inventory.h"
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

WorldSave world_defaultSave() {
    return (WorldSave) {
        .vernum = VERNUM,
        .initialCameraPosition = coords_defaultCartesian(),
        .initialCameraHeading = coords_defaultPolar(),
        .inventory = inventory_default(),
        .world = world_default()
    };
}

unsigned int world_getSaveSize(WorldSave worldSave) {
    return sizeof(worldSave) + (worldSave.world.changedBlockCount * sizeof(Block));
}

void buildWorldFilePath(uint16_t* buffer, char* name) {
    char builder[30];

    strcpy(builder, WORLD_FILE_PATH_BASE);
    strcat(builder, name);
    strcat(builder, WORLD_FILE_PATH_EXT);

    for (unsigned int i = 0; i < 30; i++) {
        buffer[i] = builder[i];
    }
}

// TODO: Pointer may not be at wrap yet, so we could read earlier
int readNext(char* queue, unsigned int* pointer, int file) {
    if ((*pointer) < 1024) {
        return 0;
    }

    if (BFile_Read(file, queue, 1024, -1) < 0) {
        return -1;
    }

    (*pointer) = 0;

    return 0;
}

WorldSave world_load(char* name) {
    WorldSave worldSave = world_defaultSave();

    uint16_t worldFilePath[30];

    buildWorldFilePath(worldFilePath, name);

    int file = BFile_Open(worldFilePath, BFile_ReadOnly);

    if (file < 0) { // For example, file may not exist
        return worldSave;
    }

    unsigned int fileSize = BFile_Size(file);
    unsigned int pointer = 0;

    // FIXME: Crashes for large files over this size
    char queue[1024];

    if (BFile_Read(file, &queue, fileSize, 0) < 0) {
        goto loadEnd;
    }

    BFile_Close(file);

    unsigned int vernum = serial_decodeUnsignedInt(queue, &pointer);

    if (vernum > VERNUM) {
        goto loadEnd; // World save is from newer version than expected
    }

    // Perform backwards-compatible conversion here

    worldSave.vernum = vernum;

    worldSave.initialCameraPosition = (CartesianVector) {
        serial_decodeDouble(queue, &pointer),
        serial_decodeDouble(queue, &pointer),
        serial_decodeDouble(queue, &pointer)
    };

    worldSave.initialCameraHeading = (PolarVector) {
        serial_decodeDouble(queue, &pointer),
        serial_decodeDouble(queue, &pointer),
        serial_decodeDouble(queue, &pointer)
    };

    worldSave.inventory.gameMode = serial_decodeInt(queue, &pointer);

    for (unsigned int i = 0; i < SLOTS_IN_INVENTORY; i++) {
        worldSave.inventory.slots[i] = (InventorySlot) {
            serial_decodeUnsignedInt(queue, &pointer),
            serial_decodeUnsignedInt(queue, &pointer)
        };
    }

    worldSave.inventory.selectedHotbarSlot = serial_decodeUnsignedInt(queue, &pointer);

    unsigned int targetBlockCount = serial_decodeUnsignedInt(queue, &pointer);

    if (readNext(queue, &pointer, file) < 0) {
        goto loadEnd;
    }

    for (unsigned int i = 0; i < targetBlockCount; i++) {
        world_addBlock(&(worldSave.world), (Block) {
            (CartesianVector) {
                serial_decodeDouble(queue, &pointer),
                serial_decodeDouble(queue, &pointer),
                serial_decodeDouble(queue, &pointer)
            },
            serial_decodeUnsignedInt(queue, &pointer)
        });

        if (readNext(queue, &pointer, file) < 0) {
        goto loadEnd;
    }
    }

    loadEnd:

    BFile_Close(file);

    return worldSave;
}

int world_save(WorldSave worldSave, char* name) {
    char* buffer = malloc(world_getSaveSize(worldSave));

    memcpy(buffer, &worldSave, world_getSaveSize(worldSave));

    unsigned int pointer = sizeof(WorldSave) - sizeof(&(worldSave.world));

    for (unsigned int i = 0; i < worldSave.world.changedBlockCount; i++) {
        char blockBuffer[sizeof(Block)];

        memcpy(blockBuffer, &(worldSave.world.changedBlocks[i]), sizeof(Block));

        for (unsigned int j = 0; j < sizeof(Block); j++) {
            buffer[pointer++] = blockBuffer[j];
        }
    }

    cpu_atomic_start();

    int status;
    int size = world_getSaveSize(worldSave);

    uint16_t worldFilePath[30];

    buildWorldFilePath(worldFilePath, name);

    BFile_Remove(worldFilePath);
    BFile_Create(worldFilePath, BFile_File, &size);

    int file = BFile_Open(worldFilePath, BFile_WriteOnly);

    if (file < 0) {
        status = file;

        goto saveEnd; // Something went wrong
    }

    status = BFile_Write(file, buffer, size);

    BFile_Close(file);

    saveEnd:

    free(buffer);

    cpu_atomic_end();

    return status;
}