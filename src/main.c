#include <gint/gint.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>
#include <gint/timer.h>

#include "flags.h"
#include "common.h"
#include "textures.h"
#include "world.h"
#include "items.h"
#include "camera.h"
#include "physics.h"
#include "inventory.h"
#include "profiling.h"

extern bopti_image_t img_logo;

World world;
Camera candidateCamera;
Inventory inventory;
bool shouldDestroyNextBlock = false;
bool shouldPlaceNextBlock = false;
bool shouldJump = false;

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
        candidateCamera.heading.incl -= 5;
    }

    if (keydown(KEY_DOWN)) {
        candidateCamera.heading.incl += 5;
    }

    if (keydown(KEY_LEFT)) {
        candidateCamera.heading.ariz -= 5;
    }

    if (keydown(KEY_RIGHT)) {
        candidateCamera.heading.ariz += 5;
    }

    if (keydown(KEY_DEL)) {
        shouldDestroyNextBlock = true;
    }

    if (keydown(KEY_ACON)) {
        shouldPlaceNextBlock = true;
    }

    if (keydown(KEY_DIV)) {
        shouldJump = true;
    }

    if (keydown(KEY_F1)) {
        inventory.selectedHotbarSlot = 0;
    }

    if (keydown(KEY_F2)) {
        inventory.selectedHotbarSlot = 1;
    }

    if (keydown(KEY_F3)) {
        inventory.selectedHotbarSlot = 2;
    }

    if (keydown(KEY_F4)) {
        inventory.selectedHotbarSlot = 3;
    }

    if (keydown(KEY_F5)) {
        inventory.selectedHotbarSlot = 4;
    }

    if (keydown(KEY_F6)) {
        inventory.selectedHotbarSlot = 5;
    }

    return TIMER_CONTINUE;
}

void main() {
    bool showLogo = true;

    textures_init();
    physics_init();
    items_init();

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
                .type = BLOCK_TYPE_LEAVES
            });
        }
    }

    world_addBlock(&world, (Block) {
        .position = (CartesianVector) {1, 3, 1},
        .type = BLOCK_TYPE_LEAVES
    });

    // candidateCamera.position.x = 1;
    // candidateCamera.position.y = 2;
    // candidateCamera.position.z = 1;

    // world_addBlock(&world, (Block) {
    //     .position = (CartesianVector) {1, 0, 1},
    //     .type = BLOCK_TYPE_WOOD
    // });

    inventory.slots[0].type = BLOCK_TYPE_STONE;
    inventory.slots[0].count = 1;
    inventory.slots[1].type = BLOCK_TYPE_GRASS;
    inventory.slots[1].count = 1;
    inventory.slots[2].type = BLOCK_TYPE_WOOD;
    inventory.slots[2].count = 1;
    inventory.slots[3].type = BLOCK_TYPE_LEAVES;
    inventory.slots[3].count = 1;

    #ifdef FLAG_PROFILING
    profiling_init();
    #endif

    int timer = timer_configure(TIMER_ETMU, 1e5, GINT_CALL(getKeypresses));

    timer_start(timer);

    while (true) {
        camera = candidateCamera;

        physics_updateDelta();

        if (shouldDestroyNextBlock) {
            camera_destroySelectedBlock(&world);

            shouldDestroyNextBlock = false;
        } else if (shouldPlaceNextBlock) {
            InventorySlot slot = inventory.slots[inventory.selectedHotbarSlot];

            if (slot.count > 0 && slot.type != BLOCK_TYPE_AIR) {
                camera_placeBlockOnFace(&world, camera, slot.type);

                shouldPlaceNextBlock = false;
            }
        } else if (shouldJump) {
            physics_jump(&sim);

            shouldJump = false;
        }

        dclear(C_WHITE);

        #ifdef FLAG_PROFILING
        profiling_reset();
        #endif

        camera_render(camera, world);

        physics_update(&sim);

        if (showLogo) {
            dimage((128 - img_logo.width) / 2, 10, &img_logo);
        } else {
            inventory_renderHotbar(inventory);
        }

        #ifdef FLAG_PROFILING
        dprint(0, 0, C_BLACK, "%dFPS", 10000 / profiling_getTicks(PROFILING_RENDER_TIME));
        #endif

        dupdate();

        clearevents();

        if (keydown(KEY_MENU)) {
            gint_osmenu();
        }

        if (keydown(KEY_EXIT)) {
            showLogo = false;
        }

        #ifdef FLAG_PROFILING
        if (keydown(KEY_LOG)) {
            showProfile();
        }
        #endif
    }
}