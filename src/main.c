#include <gint/gint.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>
#include <gint/timer.h>

#include "flags.h"
#include "common.h"
#include "ui.h"
#include "config.h"
#include "textures.h"
#include "world.h"
#include "items.h"
#include "camera.h"
#include "physics.h"
#include "crafting.h"
#include "inventory.h"
#include "profiling.h"

#define ITEM_SWITCH_SHOW_TIME 300
#define BLOCK_PLACEMENT_COOLDOWN 20

extern bopti_image_t img_logo;

Config config;
World world;
Camera candidateCamera;
Inventory inventory;
bool skipKeypresses = false;
bool shouldDestroyNextBlock = false;
bool shouldPlaceNextBlock = false;
bool shouldJump = false;
bool shouldOpenInventory = false;
bool shouldOpenCrafting = false;
int lastItemSwitchTime = -ITEM_SWITCH_SHOW_TIME;
int lastDestructionStartTime = 0;
double destructionProgress = -1;
Block lastDestructionBlock = {.position = (CartesianVector) {0, 0, 0}, .type = BLOCK_TYPE_AIR};
int lastPlaceTime = -BLOCK_PLACEMENT_COOLDOWN;

#ifdef FLAG_PROFILING

void showProfile() {
    dclear(C_WHITE);

    dfont_default();
    dprint(0, 0, C_BLACK, "DEBUG PROFILING");
    dprint(0, 8, C_BLACK, "Render: %dus", profiling_getTicks(PROFILING_RENDER_TIME) * 100);
    dprint(0, 16, C_BLACK, "- W2C: %dus", profiling_getTicks(PROFILING_WORLD_TO_CAMERA) * 100);
    dprint(0, 24, C_BLACK, "  - Rotate: %dus", profiling_getTicks(PROFILING_W2C_ROTATE) * 100);
    dprint(0, 32, C_BLACK, "- O2P: %dus", profiling_getTicks(PROFILING_ORTH_TO_PERSP) * 100);
    dprint(0, 40, C_BLACK, "- Edges: %dus", profiling_getTicks(PROFILING_FIND_EDGES) * 100);
    dprint(0, 48, C_BLACK, "- Faces: %dus", profiling_getTicks(PROFILING_DRAW_FACES) * 100);

    dupdate();

    while (keydown(KEY_LOG)) {
        clearevents();
    }
}

#endif

int getKeypresses() {
    if (skipKeypresses) {
        return TIMER_CONTINUE;
    }

    if (keydown(KEY_8)) {
        camera_moveInAriz(&candidateCamera, world, 0.2, candidateCamera.heading.ariz);
    }

    if (keydown(KEY_5)) {
        camera_moveInAriz(&candidateCamera, world, -0.2, candidateCamera.heading.ariz);
    }

    if (keydown(KEY_4)) {
        camera_moveInAriz(&candidateCamera, world, 0.2, candidateCamera.heading.ariz - 90);
    }

    if (keydown(KEY_6)) {
        camera_moveInAriz(&candidateCamera, world, 0.2, candidateCamera.heading.ariz + 90);
    }

    if (keydown(KEY_UP)) {
        candidateCamera.heading.incl -= 3;
    }

    if (keydown(KEY_DOWN)) {
        candidateCamera.heading.incl += 3;
    }

    if (keydown(KEY_LEFT)) {
        candidateCamera.heading.ariz -= 3;
    }

    if (keydown(KEY_RIGHT)) {
        candidateCamera.heading.ariz += 3;
    }

    if (keydown(KEY_DEL)) {
        shouldDestroyNextBlock = true;
    } else {
        shouldDestroyNextBlock = false;
    }

    if (keydown(KEY_ACON)) {
        shouldPlaceNextBlock = true;
    }

    if (keydown(KEY_DIV)) {
        shouldJump = true;
    }

    if (keydown(KEY_F1)) {
        inventory.selectedHotbarSlot = 0;
        lastItemSwitchTime = (int)physics_getCurrentTime();
    }

    if (keydown(KEY_F2)) {
        inventory.selectedHotbarSlot = 1;
        lastItemSwitchTime = (int)physics_getCurrentTime();
    }

    if (keydown(KEY_F3)) {
        inventory.selectedHotbarSlot = 2;
        lastItemSwitchTime = (int)physics_getCurrentTime();
    }

    if (keydown(KEY_F4)) {
        inventory.selectedHotbarSlot = 3;
        lastItemSwitchTime = (int)physics_getCurrentTime();
    }

    if (keydown(KEY_F5)) {
        inventory.selectedHotbarSlot = 4;
        lastItemSwitchTime = (int)physics_getCurrentTime();
    }

    if (keydown(KEY_F6)) {
        inventory.selectedHotbarSlot = 5;
        lastItemSwitchTime = (int)physics_getCurrentTime();
    }

    if (keydown(KEY_OPTN)) {
        shouldOpenInventory = true;
    }

    if (keydown(KEY_VARS)) {
        shouldOpenCrafting = true;
    }

    return TIMER_CONTINUE;
}

bool pauseMenu(bool renderOnly) {
    unsigned int i = 0;

    // Dither the screen and make it darker with a checkerboard pattern
    for (unsigned int y = 0; y < 64; y++) {
        for (unsigned int x = 0; x < 128; x++) {
            if (i % 2 == 0) {
                dpixel(x, y, C_BLACK);
            }

            i++;
        }

        i++;
    }

    unsigned int focus = 0;

    while (true) {
        ui_button(64 - 48, 12, 64 + 48, 12 + 12, "Resume Game", focus == 0);
        ui_button(64 - 48, 26, 64 + 48, 26 + 12, "Options...", focus == 1);
        ui_button(64 - 48, 40, 64 + 48, 40 + 12, "Save & Quit", focus == 2);

        dupdate();

        if (renderOnly) {
            return false;
        }

        switch (ui_waitForInput(&focus, 3)) {
            case INPUT_CHOICE_CONFIRM:
                if (focus == 0) {
                    return false;
                }

                if (focus == 2) {
                    return true;
                }

                break;

            case INPUT_CHOICE_EXIT:
                return false;

            case INPUT_CHOICE_MENU:
                gint_osmenu();

                break;
        }
    }
}

void startGame() {
    world = world_default();
    Camera camera;
    PhysicsSimulation sim = physics_default(&candidateCamera, &world);

    candidateCamera = camera_default();
    inventory = inventory_default();

    candidateCamera.position.x = -4;
    candidateCamera.position.y = 1;
    candidateCamera.position.z = 1;

    world_addBlock(&world, (Block) {
        .position = (CartesianVector) {1, 0, 1},
        .type = BLOCK_TYPE_WOOD
    });

    world_addBlock(&world, (Block) {
        .position = (CartesianVector) {1, 1, 1},
        .type = BLOCK_TYPE_WOOD
    });

    for (unsigned int x = 0; x < 3; x++) {
        for (unsigned int z = 0; z < 3; z++) {
            world_addBlock(&world, (Block) {
                .position = (CartesianVector) {x, 2, z},
                .type = (x == 1 && z == 1) ? BLOCK_TYPE_WOOD : BLOCK_TYPE_LEAVES
            });
        }
    }

    world_addBlock(&world, (Block) {
        .position = (CartesianVector) {1, 3, 1},
        .type = BLOCK_TYPE_LEAVES
    });

    inventory.slots[0].type = BLOCK_TYPE_STONE;
    inventory.slots[0].count = MAX_COUNT_IN_SLOT;
    inventory.slots[1].type = BLOCK_TYPE_GRASS;
    inventory.slots[1].count = MAX_COUNT_IN_SLOT;

    int timer = timer_configure(TIMER_ETMU, 1e5, GINT_CALL(getKeypresses));

    timer_start(timer);

    while (true) {
        camera = candidateCamera;

        physics_updateDelta();

        if (shouldDestroyNextBlock) {
            Block selectedBlock = camera_getSelectedBlock();

            if (selectedBlock.type != BLOCK_TYPE_AIR && coords_equalCartesian(lastDestructionBlock.position, selectedBlock.position)) {
                int destructionProgressTime = (physics_getCurrentTime() - lastDestructionStartTime) * 1e4;
                int destructionTotalTime = items_getDestructionTime(lastDestructionBlock.type, inventory.slots[inventory.selectedHotbarSlot].type);

                destructionProgress = (double)destructionProgressTime / destructionTotalTime;

                if (inventory.gameMode == GAME_MODE_CREATIVE || destructionProgressTime > destructionTotalTime) {
                    int type = camera_destroySelectedBlock(&world);

                    if (inventory.gameMode == GAME_MODE_SURVIVAL) {
                        if (items_isTool(inventory.slots[inventory.selectedHotbarSlot].type)) {
                            inventory.slots[inventory.selectedHotbarSlot].count--;
                        }

                        inventory_addFromBlockType(&inventory, type);
                    }
                }
            } else {
                lastDestructionBlock = selectedBlock;
                lastDestructionStartTime = physics_getCurrentTime();
                destructionProgress = -1;
            }
        } else if (shouldPlaceNextBlock) {
            if (physics_getCurrentTime() - lastPlaceTime > BLOCK_PLACEMENT_COOLDOWN) {
                InventorySlot slot = inventory.slots[inventory.selectedHotbarSlot];

                if (slot.count > 0 && slot.type != BLOCK_TYPE_AIR) {
                    bool success = camera_placeBlockOnFace(&world, camera, slot.type);

                    if (inventory.gameMode == GAME_MODE_SURVIVAL && success) {
                        inventory.slots[inventory.selectedHotbarSlot].count--;
                    }
                }

                lastPlaceTime = physics_getCurrentTime();
            }
        } else if (shouldJump) {
            physics_jump(&sim);
        } else if (shouldOpenInventory) {
            skipKeypresses = true;

            inventory_open(&inventory);

            skipKeypresses = false;
        } else if (shouldOpenCrafting) {
            skipKeypresses = true;

            inventory_openCrafting(&inventory, camera_getSelectedBlock().type != BLOCK_TYPE_CRAFTING_TABLE);

            skipKeypresses = false;
        }

        if (!shouldDestroyNextBlock) {
            lastDestructionStartTime = physics_getCurrentTime();
            destructionProgress = -1;
        }

        shouldPlaceNextBlock = false;
        shouldJump = false;
        shouldOpenInventory = false;
        shouldOpenCrafting = false;

        dclear(C_WHITE);

        #ifdef FLAG_PROFILING
        profiling_reset();
        #endif

        camera_render(camera, world);

        physics_update(&sim);

        inventory_renderHotbar(inventory, (int)physics_getCurrentTime() - lastItemSwitchTime < ITEM_SWITCH_SHOW_TIME);

        if (inventory.gameMode == GAME_MODE_SURVIVAL && destructionProgress >= 0) {
            ui_progressBar(127 - 24, 1, 127 - 1, 11, destructionProgress);
        }

        #ifdef FLAG_PROFILING
        dprint(0, 0, C_BLACK, "%dFPS", (int)1e4 / profiling_getTicks(PROFILING_RENDER_TIME));
        #endif

        dupdate();

        clearevents();

        if (keydown(KEY_MENU) || keydown(KEY_EXIT)) {
            skipKeypresses = true;

            if (keydown(KEY_MENU)) {
                pauseMenu(true);

                gint_osmenu();
            }

            if (pauseMenu(false)) {
                return;
            }

            skipKeypresses = false;
        }

        #ifdef FLAG_PROFILING
        if (keydown(KEY_LOG)) {
            showProfile();
        }
        #endif
    }
}

void main() {
    textures_init();
    physics_init();
    items_init();
    crafting_init();

    #ifdef FLAG_PROFILING
    profiling_init();
    #endif

    config = config_default();

    unsigned int focus = 0;

    while (true) {
        dclear(C_WHITE);
        dimage((128 - img_logo.width) / 2, 2, &img_logo);
        dprint_opt(0, 63, focus == 2 ? C_WHITE : C_BLACK, focus == 2 ? C_BLACK : C_NONE, DTEXT_LEFT, DTEXT_BOTTOM, "V%s", VERSION);

        #ifdef FLAG_PROFILING
        dtext_opt(127, 63, C_BLACK, C_NONE, DTEXT_RIGHT, DTEXT_BOTTOM, "DEBUG BUILD", -1);
        #endif

        ui_button(64 - 32, 22, 64 + 32, 22 + 12, "Play", focus == 0);
        ui_button(64 - 32, 36, 64 + 32, 36 + 12, "Options...", focus == 1);

        dupdate();

        switch (ui_waitForInput(&focus, 3)) {
            case INPUT_CHOICE_CONFIRM:
                if (focus == 0) {
                    startGame();
                }

                break;

            case INPUT_CHOICE_MENU:
                gint_osmenu();

                break;
        }
    }
}