#include <gint/display.h>
#include <gint/display-fx.h>

#include "common.h"
#include "items.h"
#include "world.h"

extern bopti_image_t img_item_stone;
extern bopti_image_t img_item_grass;
extern bopti_image_t img_item_dirt;
extern bopti_image_t img_item_cobblestone;
extern bopti_image_t img_item_plank;
extern bopti_image_t img_item_wood;
extern bopti_image_t img_item_leaves;
extern bopti_image_t img_item_craftingTable;
extern bopti_image_t img_item_woodenAxe;
extern bopti_image_t img_item_stick;

bopti_image_t* items_thumbnails[THUMBNAILS_COUNT];

unsigned int items_thumbnailMappings[THUMBNAILS_COUNT] = {
    BLOCK_TYPE_STONE,
    BLOCK_TYPE_GRASS,
    BLOCK_TYPE_DIRT,
    BLOCK_TYPE_COBBLESTONE,
    BLOCK_TYPE_PLANK,
    BLOCK_TYPE_WOOD,
    BLOCK_TYPE_LEAVES,
    BLOCK_TYPE_CRAFTING_TABLE,
    ITEM_TYPE_WOODEN_AXE,
    ITEM_TYPE_STICK
};

void items_init() {
    items_thumbnails[0] = &img_item_stone;
    items_thumbnails[1] = &img_item_grass;
    items_thumbnails[2] = &img_item_dirt;
    items_thumbnails[3] = &img_item_cobblestone;
    items_thumbnails[4] = &img_item_plank;
    items_thumbnails[5] = &img_item_wood;
    items_thumbnails[6] = &img_item_leaves;
    items_thumbnails[7] = &img_item_craftingTable;
    items_thumbnails[8] = &img_item_woodenAxe;
    items_thumbnails[9] = &img_item_stick;
}

bool items_isPlaceable(int blockType) {
    return blockType < 256;
}

bool items_isTool(int itemType) {
    switch (itemType) {
        case ITEM_TYPE_WOODEN_AXE: return true;
        default: return false;
    }
}

int items_getDestructionTime(int blockType, int toolItemType) {
    switch (blockType) {
        case BLOCK_TYPE_LEAVES: return 100 MSEC;

        case BLOCK_TYPE_WOOD:
        case BLOCK_TYPE_PLANK:
            if (toolItemType == ITEM_TYPE_WOODEN_AXE) return 300 MSEC;

            return 1 SEC;

        case BLOCK_TYPE_CRAFTING_TABLE:
            if (toolItemType == ITEM_TYPE_WOODEN_AXE) return 600 MSEC;

            return 2 SEC;

        default: return 500 MSEC;
    }
}

char* items_getItemName(int blockType) {
    switch (blockType) {
        case BLOCK_TYPE_AIR: return "Air";
        case BLOCK_TYPE_STONE: return "Stone";
        case BLOCK_TYPE_GRASS: return "Grass";
        case BLOCK_TYPE_DIRT: return "Dirt";
        case BLOCK_TYPE_COBBLESTONE: return "Cobblestone";
        case BLOCK_TYPE_PLANK: return "Wood Plank";
        case BLOCK_TYPE_WOOD: return "Wood";
        case BLOCK_TYPE_LEAVES: return "Leaves";
        case BLOCK_TYPE_CRAFTING_TABLE: return "Crafting Table";
        case ITEM_TYPE_WOODEN_AXE: return "Wooden Axe";
        case ITEM_TYPE_STICK: return "Stick";
        default: return "Unknown Item";
    }
}

char* items_getItemNameShort(int blockType) {
    switch (blockType) {
        case BLOCK_TYPE_AIR: return "Air";
        case BLOCK_TYPE_STONE: return "Stone";
        case BLOCK_TYPE_GRASS: return "Grass";
        case BLOCK_TYPE_DIRT: return "Dirt";
        case BLOCK_TYPE_COBBLESTONE: return "Cblstn";
        case BLOCK_TYPE_PLANK: return "Wood Plk";
        case BLOCK_TYPE_WOOD: return "Wood";
        case BLOCK_TYPE_LEAVES: return "Leaves";
        case BLOCK_TYPE_CRAFTING_TABLE: return "Craf Tbl";
        case ITEM_TYPE_WOODEN_AXE: return "Wood Axe";
        case ITEM_TYPE_STICK: return "Stick";
        default: return "Unknown";
    }
}