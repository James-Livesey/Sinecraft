#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>

#include "inventory.h"
#include "world.h"
#include "items.h"
#include "crafting.h"
#include "physics.h"

extern bopti_image_t img_slot;
extern bopti_image_t img_slotSelected;
extern bopti_image_t img_slotSource;
extern bopti_image_t img_craftArrow;

Inventory inventory_default() {
    Inventory inventory = {
        .gameMode = 0,
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

void inventory_addFromBlockType(Inventory* inventory, unsigned int type) {
    if (type == BLOCK_TYPE_AIR) {
        return;
    }

    for (unsigned int i = 0; i < SLOTS_IN_INVENTORY; i++) {
        if (inventory->slots[i].type == type && inventory->slots[i].count < MAX_COUNT_IN_SLOT) {
            inventory->slots[i].count++;

            return;
        }

        if (inventory->slots[i].count == 0 || inventory->slots[i].type == BLOCK_TYPE_AIR) {
            inventory->slots[i].type = type;
            inventory->slots[i].count = 1;

            return;
        }
    }
}

void inventory_renderItem(int x, int y, unsigned int type) {
    for (unsigned int i = 0; i < THUMBNAILS_COUNT; i++) {
        if (items_thumbnailMappings[i] == type) {
            dimage(x, y, items_thumbnails[i]);
        }
    }
}

void inventory_renderSlot(int x, int y, InventorySlot slot, bool selected, bool source) {
    dimage(x - 1, y - 1, selected ? &img_slotSelected : (source ? &img_slotSource : &img_slot));

    if (slot.count > 0 && slot.type != BLOCK_TYPE_AIR) {
        inventory_renderItem(x + 2, y + 2, slot.type);
    }
}

void inventory_renderRow(Inventory inventory, unsigned int offset, int y, int selected, int source) {
    unsigned int leftmostX = (128 - (SLOTS_IN_ROW * (SLOT_WIDTH + 1))) / 2;

    for (unsigned int i = 0; i < SLOTS_IN_ROW; i++) {
        int offsetI = (int)(i + offset);
        unsigned int x = leftmostX + ((SLOT_WIDTH + 1) * i);

        inventory_renderSlot(x, y, inventory.slots[i + offset], offsetI == selected, offsetI == source);
    }
}

void inventory_renderHotbar(Inventory inventory, bool showItemDetails) {
    inventory_renderRow(inventory, 0, 63 - SLOT_HEIGHT, inventory.selectedHotbarSlot, -1);

    if (showItemDetails) {
        InventorySlot selectedSlot = inventory.slots[inventory.selectedHotbarSlot];

        if (selectedSlot.count == 0 || selectedSlot.type == BLOCK_TYPE_AIR) {
            return;
        }

        dprint_opt(
            64, 64 - SLOT_HEIGHT - 12, C_INVERT, C_NONE, DTEXT_CENTER, DTEXT_TOP,
            "%s (%d)",
            items_getItemName(selectedSlot.type),
            selectedSlot.count
        );
    }
}

void inventory_renderSurvival(Inventory inventory, int selected, int source, bool showItemDetails) {
    drect(22, 0, 104, 63, C_WHITE);
    dvline(22, C_BLACK);
    dvline(104, C_BLACK);

    InventorySlot selectedSlot = inventory.slots[source >= 0 ? source : selected];

    if (selectedSlot.count == 0 || selectedSlot.type == BLOCK_TYPE_AIR) {
        showItemDetails = false;
    }

    if (showItemDetails) {
        dprint(25, 1, C_BLACK, "%s (%d)", items_getItemName(selectedSlot.type), selectedSlot.count);
    } else {
        dtext(25, 1, C_BLACK, "Inventory");
    }

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

int inventory_handleSelection(int key, int* slot) {
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

        default:
            return SELECTION_HANDLING_NONE;
    }

    return SELECTION_HANDLING_CHANGED;
}

void inventory_open(Inventory* inventory) {
    int selectedSlot = inventory->selectedHotbarSlot;
    int sourceSlot = -1;
    bool selectionChanged = false;

    while (true) {
        inventory_renderSurvival(*inventory, selectedSlot, sourceSlot, selectionChanged);

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

                if (inventory->slots[selectedSlot].count == 0 || inventory->slots[selectedSlot].type == BLOCK_TYPE_AIR) {
                    break; // Prevent movement of empty slots since this doesn't make sense
                }

                sourceSlot = selectedSlot;
                break;

            default:
                selectionChanged |= inventory_handleSelection(key, &selectedSlot) == SELECTION_HANDLING_CHANGED;

                break;
        }
    }

    closeInventory:

    if (selectedSlot < SLOTS_IN_ROW) {
        inventory->selectedHotbarSlot = selectedSlot;
    }

    physics_updateDelta();
}

void inventory_renderCrafting(Inventory inventory, CraftingRecipe recipe, bool small) {
    drect(22, 0, 104, 63, C_WHITE);
    dvline(22, C_BLACK);
    dvline(104, C_BLACK);

    dtext(25, 1, C_BLACK, "Crafting");

    unsigned int leftmostX = small ? 32 : 25;
    unsigned int topmostY = small ? 16 : 10;

    for (unsigned int py = 0; py < (small ? 2 : 3); py++) {
        for (unsigned int px = 0; px < (small ? 2 : 3); px++) {
            unsigned int i = (py * 3) + px;
            unsigned int x = leftmostX + ((SLOT_WIDTH + 1) * px);
            unsigned int y = topmostY + ((SLOT_WIDTH + 1) * py);

            inventory_renderSlot(x, y, (InventorySlot) {.type = recipe.inputTypes[i], .count = 1}, false, false);
        }
    }

    dimage(small ? 63 : 69, 24, &img_craftArrow);

    inventory_renderSlot(small ? 84 : 90, 23, (InventorySlot) {.type = recipe.outputType, .count = recipe.outputCount}, false, false);

    dprint_opt(small ? 89 : 95, 37, C_BLACK, C_NONE, DTEXT_CENTER, DTEXT_TOP, "%d", recipe.outputCount);

    inventory_renderHotbar(inventory, false);
}

void inventory_openCrafting(Inventory* inventory, bool small) {
    // TODO: Implement actual crafting behaviour

    inventory_renderCrafting(*inventory, crafting_recipes[0], small);

    dupdate();

    getkey();

    physics_updateDelta();
}