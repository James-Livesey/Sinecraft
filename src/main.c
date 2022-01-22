#include <stdio.h>
#include <string.h>
#include <gint/gint.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>
#include <gint/timer.h>
#include <gint/bfile.h>

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
#define MESSAGE_SHOW_TIME 300
#define BLOCK_PLACEMENT_COOLDOWN 20
#define RESULTS_ON_WORLD_LISTING 5

extern bopti_image_t img_logo;
extern bopti_image_t img_fn_play;
extern bopti_image_t img_fn_new;
extern bopti_image_t img_fn_create;
extern bopti_image_t img_fn_casing;

Config config;
World world;
Camera candidateCamera;
Inventory inventory;
char worldName[MAX_WORLD_NAME_LENGTH] = "";
unsigned int worldGenerationType = WORLD_GENERATION_TYPE_DEFAULT;
int keypressTimer;
bool shouldDestroyNextBlock = false;
bool shouldPlaceNextBlock = false;
bool shouldJump = false;
bool shouldOpenInventory = false;
bool shouldOpenCrafting = false;
int lastItemSwitchTime = -ITEM_SWITCH_SHOW_TIME;
int lastBlockOutOfBoundsMessageTime = -MESSAGE_SHOW_TIME;
int lastDestructionStartTime = 0;
double destructionProgress = -1;
Block lastDestructionBlock = {.position = (CartesianVector) {0, 0, 0}, .type = BLOCK_TYPE_AIR};
int lastPlaceTime = -BLOCK_PLACEMENT_COOLDOWN;

struct WorldListingResults {
    unsigned int count;
    char names[RESULTS_ON_WORLD_LISTING][8];
    unsigned int sizes[RESULTS_ON_WORLD_LISTING];
};

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
    int actualCamSpeed = 1 + (2 * ((double)config.camSpeed / 100));

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
        candidateCamera.heading.incl -= actualCamSpeed * (config.invertY ? -1 : 1);
    }

    if (keydown(KEY_DOWN)) {
        candidateCamera.heading.incl += actualCamSpeed * (config.invertY ? -1 : 1);
    }

    if (keydown(KEY_LEFT)) {
        candidateCamera.heading.ariz -= actualCamSpeed;
    }

    if (keydown(KEY_RIGHT)) {
        candidateCamera.heading.ariz += actualCamSpeed;
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

void startKeypressTimer() {
    keypressTimer = timer_configure(TIMER_ETMU, 1e5, GINT_CALL(getKeypresses));

    timer_start(keypressTimer);
}

void stopKeypressTimer() {
    timer_stop(keypressTimer);
}

void showAlert(char* line1, char* line2, char* line3, char* line4) {
    ui_message(line1, line2, line3, line4);

    dupdate();

    while (!keydown(KEY_EXE)) {
        clearevents();
    }
}

bool loadWorld() {
    ui_message("Loading world:", worldName, "", "Please wait...");

    dupdate();

    WorldSaveStatus status = world_load(worldName);

    if (status.status == WORLD_SAVE_CANNOT_LOAD) {
        showAlert(
            "Memory ERROR",
            "Couldn't read!",
            "",
            "   Press:[EXE]"
        );

        return false;
    }

    if (status.status == WORLD_SAVE_TOO_BIG) {
        showAlert(
            "Memory ERROR",
            "World too big!",
            "Must be < 16 KiB", // Update this value from `MAX_WORLD_SIZE`
            "   Press:[EXE]"
        );

        return false;
    }

    if (status.status == WORLD_SAVE_NEWER_THAN_EXPECTED) {
        showAlert(
            "Update Sinecraft",
            "to play this",
            "world",
            "   Press:[EXE]"
        );

        return false;
    }

    candidateCamera.position = status.worldSave.initialCameraPosition;
    candidateCamera.heading = status.worldSave.initialCameraHeading;

    inventory = status.worldSave.inventory;
    world = status.worldSave.world;

    return true;
}

void saveWorld() {
    ui_message("Saving world:", worldName, "", "Please wait...");

    dupdate();

    WorldSave worldSave = world_defaultSave();

    worldSave.worldGenerationType = worldGenerationType;

    worldSave.initialCameraPosition = candidateCamera.position;
    worldSave.initialCameraHeading = candidateCamera.heading;

    worldSave.inventory = inventory;
    worldSave.world = world;

    int status = world_save(worldSave, worldName);

    if (status < 0) {
        char errorCodeLine[16];

        sprintf(errorCodeLine, "Error %d", status);

        showAlert(
            "Memory ERROR",
            "Couldn't write!",
            errorCodeLine,
            "   Press:[EXE]"
        );
    }
}

void optionsMenu() {
    unsigned int focus = 0;

    while (true) {
        dclear(C_WHITE);

        dtext(2, 1, C_BLACK, "Options");
        dhline(10, C_BLACK);

        char fovText[16];

        switch (config.fov) {
            case 60:
                strcpy(fovText, "FOV: Normal");
                break;

            case 90:
                strcpy(fovText, "FOV: Quake Pro");
                break;

            default:
                sprintf(fovText, "FOV: %d", config.fov);
                break;
        }

        ui_progressBar(64 - 48, 16, 64 + 48, 16 + 12, (double)(config.fov - 30) / 60);
        ui_progressLabel(64 - 48, 16, 64 + 48, 16 + 12, fovText, focus == 0);

        char camSpeedText[16];

        switch (config.camSpeed) {
            case 0:
                strcpy(camSpeedText, "Looking: *yawn*");
                break;

            case 200:
                strcpy(camSpeedText, "Looking: HYPER!");
                break;

            default:
                sprintf(camSpeedText, "Looking: %d%%", config.camSpeed);
                break;
        }

        ui_progressBar(64 - 48, 30, 64 + 48, 30 + 12, (double)config.camSpeed / 200);
        ui_progressLabel(64 - 48, 30, 64 + 48, 30 + 12, camSpeedText, focus == 1);

        ui_button(64 - 48, 44, 64 + 48, 44 + 12, config.invertY ? "Y Axis: Invert" : "Y Axis: Normal", focus == 2);

        dupdate();

        switch (ui_waitForInput(&focus, 3)) {
            case INPUT_CHOICE_CONFIRM:
                if (focus == 2) {
                    config.invertY = !config.invertY;
                }

                break;

            case INPUT_CHOICE_EXIT:
                while (keydown(KEY_EXIT)) {
                    clearevents();
                }

                return;

            case INPUT_CHOICE_MENU:
                gint_osmenu();

                break;

            case INPUT_CHOICE_PREVIOUS:
                if (focus == 0 && config.fov > 30) {
                    config.fov--;
                }

                if (focus == 1 && config.camSpeed > 0) {
                    config.camSpeed--;
                }

                break;

            case INPUT_CHOICE_NEXT:
                if (focus == 0 && config.fov < 90) {
                    config.fov++;
                }

                if (focus == 1 && config.camSpeed < 200) {
                    config.camSpeed++;
                }

                break;
        }
    }
}

void saveOptions() {
    ui_message("Saving...", "", "", "");

    dupdate();

    int status = config_save(config);

    if (status < 0) {
        char errorCodeLine[16];

        sprintf(errorCodeLine, "Error %d", status);

        showAlert(
            "Memory ERROR",
            "Couldn't write!",
            errorCodeLine,
            "   Press:[EXE]"
        );
    }
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

                if (focus == 1) {
                    optionsMenu();
                    saveOptions();

                    dclear(C_WHITE);
                }

                if (focus == 2) {
                    return true;
                }

                break;

            case INPUT_CHOICE_EXIT:
                while (keydown(KEY_EXIT)) {
                    clearevents();
                }

                return false;

            case INPUT_CHOICE_MENU:
                gint_osmenu();

                break;
        }
    }
}

void newWorld() {
    world = world_default();
    candidateCamera = camera_default();
    inventory = inventory_default();

    candidateCamera.position.x = 1;
    candidateCamera.position.y = 1;
    candidateCamera.position.z = 1;

    world_addBlock(&world, (Block) {
        .position = (CartesianVector) {6, 0, 1},
        .type = BLOCK_TYPE_WOOD
    });

    world_addBlock(&world, (Block) {
        .position = (CartesianVector) {6, 1, 1},
        .type = BLOCK_TYPE_WOOD
    });

    for (unsigned int x = 0; x < 3; x++) {
        for (unsigned int z = 0; z < 3; z++) {
            world_addBlock(&world, (Block) {
                .position = (CartesianVector) {x + 5, 2, z},
                .type = (x == 1 && z == 1) ? BLOCK_TYPE_WOOD : BLOCK_TYPE_LEAVES
            });
        }
    }

    world_addBlock(&world, (Block) {
        .position = (CartesianVector) {6, 3, 1},
        .type = BLOCK_TYPE_LEAVES
    });

    inventory.slots[0].type = BLOCK_TYPE_STONE;
    inventory.slots[0].count = MAX_COUNT_IN_SLOT;
    inventory.slots[1].type = BLOCK_TYPE_GRASS;
    inventory.slots[1].count = MAX_COUNT_IN_SLOT;
}

void startGame() {
    Camera camera;
    PhysicsSimulation sim = physics_default(&candidateCamera, &world);

    startKeypressTimer();

    while (true) {
        candidateCamera.fov = config.fov;

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
                    int status = camera_placeBlockOnFace(&world, camera, slot.type);

                    if (inventory.gameMode == GAME_MODE_SURVIVAL && status == BLOCK_PLACE_SUCCESS) {
                        inventory.slots[inventory.selectedHotbarSlot].count--;
                    }

                    if (status == BLOCK_PLACE_OUT_OF_BOUNDS) {
                        lastBlockOutOfBoundsMessageTime = physics_getCurrentTime();
                    }
                }

                lastPlaceTime = physics_getCurrentTime();
            }
        } else if (shouldJump) {
            physics_jump(&sim);
        } else if (shouldOpenInventory) {
            stopKeypressTimer();

            inventory_open(&inventory);

            startKeypressTimer();
        } else if (shouldOpenCrafting) {
            stopKeypressTimer();

            inventory_openCrafting(&inventory, camera_getSelectedBlock().type != BLOCK_TYPE_CRAFTING_TABLE);

            startKeypressTimer();
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

        if ((int)physics_getCurrentTime() - lastBlockOutOfBoundsMessageTime < MESSAGE_SHOW_TIME) {
            ui_message(
                "Block placement",
                "outside of world",
                "border!",
                ""
            );
        }

        #ifdef FLAG_PROFILING
        dprint(0, 0, C_BLACK, "%dFPS", (int)1e4 / profiling_getTicks(PROFILING_RENDER_TIME));
        #endif

        dupdate();

        clearevents();

        if (keydown(KEY_MENU) || keydown(KEY_EXIT)) {
            stopKeypressTimer();

            if (keydown(KEY_MENU)) {
                pauseMenu(true);

                gint_osmenu();
            }

            if (pauseMenu(false)) {
                return;
            }

            startKeypressTimer();
        }

        #ifdef FLAG_PROFILING
        if (keydown(KEY_LOG)) {
            showProfile();
        }
        #endif
    }
}

bool newWorldMenu() {
    unsigned int focus = 0;

    char newWorldName[MAX_WORLD_NAME_LENGTH + 1] = ""; // Add 1 for null terminator required for `ui_inputEvent`
    unsigned int caretPosition = 0;
    unsigned int gameModeChoice = 0;

    ui_setModifierState(MODIFIER_STATE_ALPHA_LOCK);
    ui_setCasingState(CASING_STATE_UPPER);

    while (true) {
        dclear(C_WHITE);

        dtext(2, 1, C_BLACK, "Create New World");
        dhline(10, C_BLACK);

        dtext_opt(1, 12 + 6, C_BLACK, C_NONE, DTEXT_LEFT, DTEXT_CENTER, "Name", -1);
        ui_input(60, 12, 126, 12 + 12, newWorldName, caretPosition, focus == 0);

        char gameModeText[16];

        switch (gameModeChoice) {
            case 0:
                strcpy(gameModeText, "Survival");
                break;

            case 1:
                strcpy(gameModeText, "Creative");
                break;

            case 2:
                strcpy(gameModeText, "Superflat");
                break;
        }

        dtext_opt(1, 26 + 6, C_BLACK, C_NONE, DTEXT_LEFT, DTEXT_CENTER, "Game Mode", -1);
        ui_button(60, 26, 126, 26 + 12, gameModeText, focus == 1);

        switch (gameModeChoice) {
            case 0:
                dtext_opt(64, 42, C_BLACK, C_NONE, DTEXT_CENTER, DTEXT_TOP, "Crafting and health", -1);
                break;

            case 1:
                dtext_opt(64, 42, C_BLACK, C_NONE, DTEXT_CENTER, DTEXT_TOP, "Unlimited blocks", -1);
                break;

            case 2:
                dtext_opt(64, 42, C_BLACK, C_NONE, DTEXT_CENTER, DTEXT_TOP, "A flat creative world", -1);
                break;
        }

        ui_functionIndicator(5, &img_fn_casing);
        ui_functionIndicator(6, &img_fn_create);

        dupdate();

        switch (ui_waitForInput(&focus, 2)) {
            case INPUT_CHOICE_CONFIRM:
                if (focus == 1) {
                    gameModeChoice = (gameModeChoice + 1) % 3;
                }

                break;

            case INPUT_CHOICE_EXIT:
                return false;

            case INPUT_CHOICE_FN:
                if (ui_getFnKey() == 5) {
                    ui_toggleCasingState();
                }

                if (ui_getFnKey() == 6) {
                    if (strlen(newWorldName) == 0) {
                        showAlert(
                            "Please enter a",
                            "name for the new",
                            "world",
                            "   Press:[EXE]"
                        );

                        break;
                    }

                    worldGenerationType = gameModeChoice == 2 ? WORLD_GENERATION_TYPE_SUPERFLAT : WORLD_GENERATION_TYPE_DEFAULT;

                    inventory.gameMode = gameModeChoice == 0 ? GAME_MODE_SURVIVAL : GAME_MODE_CREATIVE;

                    strncpy(worldName, newWorldName, MAX_WORLD_NAME_LENGTH);

                    return true;
                }

                break;

            case INPUT_CHOICE_MENU:
                gint_osmenu();

                break;

            default:
                ui_inputEvent(newWorldName, &caretPosition, MAX_WORLD_NAME_LENGTH, true, focus == 0);

                break;
        }
    }
}

unsigned int getWorldCount() {
    unsigned int count = 0;
    bool foundFirst = false;
    int searchHandle;

    while (true) {
        uint16_t foundPath[30];
        struct BFile_FileInfo foundFileInfo;
        int searchStatus = 0;

        if (!foundFirst) {
            searchStatus = BFile_FindFirst(WORLD_FILE_SEARCH_PATTERN, &searchHandle, foundPath, &foundFileInfo);
            foundFirst = true;
        } else {
            searchStatus = BFile_FindNext(searchHandle, foundPath, &foundFileInfo);
        }

        if (searchStatus < 0) {
            break;
        }

        count++;
    }

    return count;
}

struct WorldListingResults getWorldListing(unsigned int offset) {
    struct WorldListingResults results = {
        .count = 0
    };

    int currentIndex = -offset - 1;
    bool foundFirst = false;
    int searchHandle;

    while (true) {
        uint16_t foundPath[30];
        struct BFile_FileInfo foundFileInfo;
        int searchStatus;

        currentIndex++;

        if (currentIndex >= RESULTS_ON_WORLD_LISTING) {
            break;
        }

        if (!foundFirst) {
            searchStatus = BFile_FindFirst(WORLD_FILE_SEARCH_PATTERN, &searchHandle, foundPath, &foundFileInfo);
            foundFirst = true;
        } else {
            searchStatus = BFile_FindNext(searchHandle, foundPath, &foundFileInfo);
        }

        if (currentIndex < 0) {
            continue;
        }

        strcpy(results.names[currentIndex], "");

        results.sizes[currentIndex] = 0;

        if (searchStatus < 0) {
            break;
        }

        for (unsigned int i = 0; i < 8; i++) {
            char nextChar = foundPath[i];

            if (nextChar == '.') {
                break; // Reached extension part
            }

            results.names[currentIndex][i] = nextChar;
        }

        results.sizes[currentIndex] = (unsigned int)foundFileInfo.file_size;
        results.count++;
    }

    BFile_FindClose(searchHandle);

    return results;
}

void worldMenu() {
    unsigned int focus = 0;
    unsigned int scrollPos = 0;

    struct WorldListingResults listing = getWorldListing(scrollPos);
    unsigned int worldCount = getWorldCount();

    while (true) {
        dclear(C_WHITE);

        dtext(2, 1, C_BLACK, "Select World");
        dhline(10, C_BLACK);

        bool scrollingChanged = false;

        while (focus < scrollPos) {
            scrollPos--;
            scrollingChanged = true;
        }

        while (focus > scrollPos + RESULTS_ON_WORLD_LISTING - 1) {
            scrollPos++;
            scrollingChanged = true;
        }

        if (scrollingChanged) {
            listing = getWorldListing(scrollPos);
        }

        for (unsigned int i = 0; i < listing.count; i++) {
            if (i == focus - scrollPos) {
                drect(0, 12 + (i * 8), 127, 19 + (i * 8), C_BLACK);
            }

            dtext(6, 12 + (i * 8), C_INVERT, listing.names[i]);
            dprint(127 - (5 * 6), 12 + (i * 8), C_INVERT, "%d", listing.sizes[i]);
        }

        if (worldCount == 0) {
            dtext_opt(64, 32, C_BLACK, C_NONE, DTEXT_CENTER, DTEXT_CENTER, "(No worlds yet!)");
        } else {
            ui_functionIndicator(1, &img_fn_play);
        }

        ui_functionIndicator(3, &img_fn_new);

        dupdate();

        switch (ui_waitForInput(&focus, worldCount)) {
            case INPUT_CHOICE_CONFIRM:
                if (worldCount == 0) {
                    break;
                }

                strcpy(worldName, listing.names[focus - scrollPos]);

                bool success = loadWorld();

                if (!success) {
                    break;
                }

                startGame();
                saveWorld();

                return; // So that quitting a world returns to main menu

            case INPUT_CHOICE_EXIT:
                return;

            case INPUT_CHOICE_FN:
                if (ui_getFnKey() == 1 || ui_getFnKey() == 3) {
                    if (ui_getFnKey() == 1) {
                        if (worldCount == 0) {
                            break;
                        }

                        strcpy(worldName, listing.names[focus - scrollPos]);

                        bool success = loadWorld();

                        if (!success) {
                            break;
                        }
                    } else if (ui_getFnKey() == 3) {
                        strcpy(worldName, "World"); // Just in case world name somehow doesn't get assigned to

                        bool success = newWorldMenu();

                        if (!success) {
                            break;
                        }

                        newWorld();
                    }

                    startGame();
                    saveWorld();

                    return;
                }

                break;

            case INPUT_CHOICE_MENU:
                gint_osmenu();

                break;
        }
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

    config = config_load();

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
                    worldMenu();
                }

                if (focus == 1) {
                    optionsMenu();
                    saveOptions();
                }

                break;

            case INPUT_CHOICE_MENU:
                gint_osmenu();

                break;
        }
    }
}