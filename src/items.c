#include <gint/display.h>
#include <gint/display-fx.h>

#include "items.h"
#include "world.h"

extern bopti_image_t img_item_stone;
extern bopti_image_t img_item_grass;
extern bopti_image_t img_item_dirt;
extern bopti_image_t img_item_cobblestone;
extern bopti_image_t img_item_plank;
extern bopti_image_t img_item_wood;
extern bopti_image_t img_item_leaves;

bopti_image_t* items_thumbnails[THUMBNAILS_COUNT];

unsigned int items_thumbnailMappings[THUMBNAILS_COUNT] = {
    BLOCK_TYPE_STONE,
    BLOCK_TYPE_GRASS,
    BLOCK_TYPE_DIRT,
    BLOCK_TYPE_COBBLESTONE,
    BLOCK_TYPE_PLANK,
    BLOCK_TYPE_WOOD,
    BLOCK_TYPE_LEAVES
};

void items_init() {
    items_thumbnails[0] = &img_item_stone;
    items_thumbnails[1] = &img_item_grass;
    items_thumbnails[2] = &img_item_dirt;
    items_thumbnails[3] = &img_item_cobblestone;
    items_thumbnails[4] = &img_item_plank;
    items_thumbnails[5] = &img_item_wood;
    items_thumbnails[6] = &img_item_leaves;
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
        default: return "Unknown Item";
    }
}