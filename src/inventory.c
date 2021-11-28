#include <gint/display.h>
#include <gint/display-fx.h>

#include "inventory.h"
#include "world.h"

extern bopti_image_t img_slot;

Inventory inventory_default() {
    Inventory inventory = {
        .selectedHotbarSlot = 0
    };

    for (unsigned int i = 0; i < SLOTS_IN_INVENTORY; i++) {
        inventory.slots[i] = (InventorySlot) {
            .type = BLOCK_TYPE_AIR,
            .count = 0
        };
    }

    return inventory;
}

void inventory_renderHotbar(Inventory inventory) {
    unsigned int leftmostX = (128 - (SLOTS_IN_HOTBAR * (img_slot.width + 1))) / 2;

    for (unsigned int i = 0; i < SLOTS_IN_HOTBAR; i++) {
        unsigned int x = leftmostX + ((img_slot.width + 1) * i);

        dimage(x, 64 - img_slot.height - 1, &img_slot);

        if (i == inventory.selectedHotbarSlot) {
            drect_border(x - 1, 64 - img_slot.height - 2, x + img_slot.width, 64 - 1, C_NONE, 2, C_BLACK);
        }

        // TODO: Implement actual rendering of inventory items
    }
}