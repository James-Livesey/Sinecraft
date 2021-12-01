#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>

#include "inventory.h"
#include "world.h"
#include "items.h"
#include "physics.h"

extern bopti_image_t img_slot;
extern bopti_image_t img_slotSelected;
extern bopti_image_t img_slotSource;

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

void inventory_renderRow(Inventory inventory, unsigned int offset, int y, int selected, int source) {
    unsigned int leftmostX = (128 - (SLOTS_IN_ROW * (SLOT_WIDTH + 1))) / 2;

    for (unsigned int i = 0; i < SLOTS_IN_ROW; i++) {
        int offsetI = (int)(i + offset);
        unsigned int x = leftmostX + ((SLOT_WIDTH + 1) * i);
        bool isSelected = offsetI == selected;
        bool isSource = offsetI == source;

        dimage(x - 1, y - 1, isSelected ? &img_slotSelected : (isSource ? &img_slotSource : &img_slot));

        if (inventory.slots[i + offset].count > 0 && inventory.slots[i + offset].type != BLOCK_TYPE_AIR) {
            inventory_renderItem(x + 2, y + 2, inventory.slots[i + offset].type);
        }
    }
}

void inventory_renderHotbar(Inventory inventory) {
    inventory_renderRow(inventory, 0, 63 - SLOT_HEIGHT, inventory.selectedHotbarSlot, -1);
}

void inventory_renderSurvival(Inventory inventory, int selected, int source) {
    drect(22, 0, 104, 63, C_WHITE);
    dvline(22, C_BLACK);
    dvline(104, C_BLACK);
    dtext(25, 1, C_BLACK, "Inventory");

    const int ROW_OFFSET_HEIGHT = SLOT_HEIGHT + 1;
    const int ROW_Y_VALUES[] = {
        63 - SLOT_HEIGHT,
        10 + (ROW_OFFSET_HEIGHT * 2),
        10 + (ROW_OFFSET_HEIGHT * 1),
        10
    };

    for (unsigned int row = 0; row < SLOTS_IN_INVENTORY / SLOTS_IN_ROW; row++) {
        inventory_renderRow(inventory, row * SLOTS_IN_ROW, ROW_Y_VALUES[row], selected, source);
    }
}

void inventory_handleSelection(int key, int* slot) {
    switch (key) {
        case KEY_UP:
            if (*slot > SLOTS_IN_INVENTORY - SLOTS_IN_ROW - 1) {
                break;
            }

            *slot += SLOTS_IN_ROW;
            break;

        case KEY_DOWN:
            if (*slot < SLOTS_IN_ROW) {
                break;
            }

            *slot -= SLOTS_IN_ROW;
            break;

        case KEY_LEFT:
            if (*slot % SLOTS_IN_ROW == 0) {
                if (*slot > SLOTS_IN_INVENTORY - SLOTS_IN_ROW - 1) {
                    break;
                }

                *slot += (2 * SLOTS_IN_ROW) - 1;

                break;
            }

            (*slot)--;
            break;

        case KEY_RIGHT:
            if (*slot % SLOTS_IN_ROW == SLOTS_IN_ROW - 1) {
                if (*slot < SLOTS_IN_ROW) {
                    break;
                }

                *slot -= (2 * SLOTS_IN_ROW) - 1;

                break;
            }

            (*slot)++;
            break;

        case KEY_F1:
        case KEY_F2:
        case KEY_F3:
        case KEY_F4:
        case KEY_F5:
        case KEY_F6:
            *slot = key - KEY_F1;
            break;
    }
}

void inventory_open(Inventory* inventory) {
    int selectedSlot = inventory->selectedHotbarSlot;
    int sourceSlot = -1;

    while (true) {
        inventory_renderSurvival(*inventory, selectedSlot, sourceSlot);

        dupdate();

        int key = getkey().key;

        switch (key) {
            case KEY_EXIT:
                goto closeInventory;

            case KEY_EXE:
                if (sourceSlot >= 0) {
                    InventorySlot temp = inventory->slots[selectedSlot];

                    inventory->slots[selectedSlot] = inventory->slots[sourceSlot];
                    inventory->slots[sourceSlot] = temp;

                    sourceSlot = -1;

                    break;
                }

                sourceSlot = selectedSlot;
                break;

            default:
                inventory_handleSelection(key, &selectedSlot);
                break;
        }

        while (selectedSlot >= SLOTS_IN_INVENTORY) {
            selectedSlot -= SLOTS_IN_ROW;
        }

        while (selectedSlot < 0) {
            selectedSlot += SLOTS_IN_ROW;
        }
    }

    closeInventory:

    if (selectedSlot < SLOTS_IN_ROW) {
        inventory->selectedHotbarSlot = selectedSlot;
    }

    physics_updateDelta();
}