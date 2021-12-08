#ifndef INVENTORY_H_
#define INVENTORY_H_

#include "crafting.h"

#define SLOTS_IN_ROW 6
#define SLOTS_IN_INVENTORY 24
#define MAX_COUNT_IN_SLOT 16
#define SLOT_WIDTH 12
#define SLOT_HEIGHT 12

enum {
    SELECTION_HANDLING_NONE,
    SELECTION_HANDLING_CHANGED
};

enum {
    GAME_MODE_SURVIVAL = 0
};

typedef struct {
    unsigned int type;
    unsigned int count;
} InventorySlot;

typedef struct {
    int gameMode;
    InventorySlot slots[SLOTS_IN_INVENTORY];
    unsigned int selectedHotbarSlot;
} Inventory;

typedef struct {
    bool itemsAvailable[9];
    bool craftableInSize;
    bool hasAtLeastOneItem;
} InventoryCraftingStatus;

Inventory inventory_default();

void inventory_addFromBlockType(Inventory* inventory, unsigned int type);
bool inventory_removeFromBlockType(Inventory* inventory, unsigned int type);

InventoryCraftingStatus inventory_canCraft(Inventory inventory, CraftingRecipe recipe, bool small);
void inventory_craft(Inventory* inventory, CraftingRecipe recipe);

void inventory_renderItem(int x, int y, unsigned int type);
void inventory_renderSlot(int x, int y, InventorySlot slot, bool selected, bool source);
void inventory_renderRow(Inventory inventory, unsigned int offset, int y, int selected, int source);
void inventory_renderHotbar(Inventory inventory, bool showItemDetails);
void inventory_renderSurvival(Inventory inventory, int selected, int source, bool showItemDetails);

int inventory_handleSelection(int key, int* slot);
void inventory_open(Inventory* inventory);

void inventory_renderCrafting(Inventory inventory, CraftingRecipe recipe, bool small, InventoryCraftingStatus status);
void inventory_renderCraftingNone(Inventory inventory);
bool inventory_craftingIsDisplayable(Inventory inventory, CraftingRecipe recipe, bool small);
int inventory_craftingFindFirstDisplayable(Inventory inventory, bool small);
void inventory_openCrafting(Inventory* inventory, bool small);

#endif