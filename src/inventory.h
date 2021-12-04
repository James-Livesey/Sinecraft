#ifndef INVENTORY_H_
#define INVENTORY_H_

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

Inventory inventory_default();

void inventory_addFromBlockType(Inventory* inventory, unsigned int type);

void inventory_renderItem(int x, int y, unsigned int type);
void inventory_renderRow(Inventory inventory, unsigned int offset, int y, int selected, int source);
void inventory_renderHotbar(Inventory inventory, bool showItemDetails);
void inventory_renderSurvival(Inventory inventory, int selected, int source, bool showItemDetails);

int inventory_handleSelection(int key, int* slot);
void inventory_open(Inventory* inventory);

#endif