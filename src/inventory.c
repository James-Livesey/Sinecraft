#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>

#include "inventory.h"
#include "world.h"
#include "items.h"

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

extern const bopti_image_t img_item_grass;

void inventory_renderItem(int x, int y, unsigned int type) {
    for (unsigned int i = 0; i < THUMBNAILS_COUNT; i++) {
        if (items_thumbnailMappings[i] == type) {
            dimage(x, y, items_thumbnails[i]);
        }
    }
}

void inventory_renderRow(Inventory inventory, unsigned int offset, int y, int selected) {
    unsigned int leftmostX = (128 - (SLOTS_IN_ROW * (img_slot.width + 1))) / 2;

    for (unsigned int i = 0; i < SLOTS_IN_ROW; i++) {
        unsigned int x = leftmostX + ((img_slot.width + 1) * i);

        dimage(x, y, &img_slot);

        if (inventory.slots[i + offset].count > 0 && inventory.slots[i + offset].type != BLOCK_TYPE_AIR) {
            inventory_renderItem(x + 2, y + 2, inventory.slots[i + offset].type);
        }

        if ((int)(i + offset) == selected) {
            drect_border(x - 1, y - 1, x + img_slot.width, y + img_slot.height, C_NONE, 2, C_BLACK);
        }
    }
}

void inventory_renderHotbar(Inventory inventory) {
    inventory_renderRow(inventory, 0, 63 - img_slot.height, inventory.selectedHotbarSlot);
}

void inventory_renderSurvival(Inventory inventory, int selected) {
    drect(22, 0, 104, 63, C_WHITE);
    dvline(22, C_BLACK);
    dvline(104, C_BLACK);
    dtext(25, 1, C_BLACK, "Inventory");

    const int ROW_OFFSET_HEIGHT = img_slot.height + 1;
    const int ROW_Y_VALUES[] = {
        63 - img_slot.height,
        10 + (ROW_OFFSET_HEIGHT * 2),
        10 + (ROW_OFFSET_HEIGHT * 1),
        10
    };

    for (unsigned int row = 0; row < SLOTS_IN_INVENTORY / SLOTS_IN_ROW; row++) {
        inventory_renderRow(inventory, row * SLOTS_IN_ROW, ROW_Y_VALUES[row], selected);
    }
}

void inventory_open(Inventory* inventory) {
    int selectedSlot = inventory->selectedHotbarSlot;

    while (true) {
        inventory_renderSurvival(*inventory, selectedSlot);

        dupdate();

        switch (getkey().key) {
            case KEY_EXIT:
                goto closeInventory;

            case KEY_UP:
                selectedSlot += SLOTS_IN_ROW;
                break;

            case KEY_DOWN:
                selectedSlot -= SLOTS_IN_ROW;
                break;

            case KEY_LEFT:
                selectedSlot--;

                if (selectedSlot % SLOTS_IN_ROW == SLOTS_IN_ROW - 1) {
                    selectedSlot += SLOTS_IN_ROW * 2;
                }

                break;

            case KEY_RIGHT:
                selectedSlot++;

                if (selectedSlot % SLOTS_IN_ROW == 0) {
                    selectedSlot -= SLOTS_IN_ROW * 2;
                }

                break;

            case KEY_F1:
                selectedSlot = 0;
                break;

            case KEY_F2:
                selectedSlot = 1;
                break;

            case KEY_F3:
                selectedSlot = 2;
                break;

            case KEY_F4:
                selectedSlot = 3;
                break;

            case KEY_F5:
                selectedSlot = 4;
                break;

            case KEY_F6:
                selectedSlot = 5;
                break;
        }

        while (selectedSlot > SLOTS_IN_INVENTORY) {
            selectedSlot -= SLOTS_IN_ROW;
        }

        while (selectedSlot < 0) {
            selectedSlot += SLOTS_IN_ROW;
        }
    }

    closeInventory:

    clearevents();
}