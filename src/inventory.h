#ifndef INVENTORY_H_
#define INVENTORY_H_

#define SLOTS_IN_HOTBAR 6
#define SLOTS_IN_INVENTORY 24

typedef struct {
    unsigned int type;
    unsigned int count;
} InventorySlot;

typedef struct {
    InventorySlot slots[SLOTS_IN_INVENTORY];
    unsigned int selectedHotbarSlot;
} Inventory;

Inventory inventory_default();

void inventory_renderHotbar(Inventory inventory);

#endif