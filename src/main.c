#include <gint/gint.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>

#include "flags.h"
#include "common.h"
#include "textures.h"
#include "world.h"
#include "camera.h"
#include "profiling.h"

extern bopti_image_t img_logo;

#ifdef FLAG_PROFILING

void showProfile() {
	dclear(C_WHITE);

	dfont_default();
	dprint(0, 0, C_BLACK, "DEBUG PROFILING");
	dprint(0, 8, C_BLACK, "Render: %dus", profiling_getTicks(PROFILING_RENDER_TIME) * 100);
	dprint(0, 16, C_BLACK, "- W2C: %dus", profiling_getTicks(PROFILING_WORLD_TO_CAMERA) * 100);
	dprint(0, 24, C_BLACK, "- O2P: %dus", profiling_getTicks(PROFILING_ORTH_TO_PERSP) * 100);
	dprint(0, 32, C_BLACK, "- Edges: %dus", profiling_getTicks(PROFILING_FIND_EDGES) * 100);
	dprint(0, 40, C_BLACK, "- Faces: %dus", profiling_getTicks(PROFILING_DRAW_FACES) * 100);

	dupdate();

	while (keydown(KEY_LOG)) {
		clearevents();
	}
}

#endif

void main() {
	bool showLogo = true;

	textures_init();

	World world = world_default();
	Camera camera = camera_default();

	camera.position.x = -4;
	camera.position.y = 1;
	camera.position.z = 1;

	for (unsigned int x = 0; x < 3; x++) {
		for (unsigned int z = 0; z < 3; z++) {
			world_addBlock(&world, (Block) {
				.position = (CartesianVector) {x, 0, z},
				.type = BLOCK_TYPE_STONE
			});
		}
	}

	world_addBlock(&world, (Block) {
		.position = (CartesianVector) {1, 1, 1},
		.type = BLOCK_TYPE_STONE
	});

	#ifdef FLAG_PROFILING
	profiling_init();
	#endif

	while (true) {
		dclear(C_WHITE);

		#ifdef FLAG_PROFILING
		profiling_reset();
		#endif

		camera_render(camera, world);

		if (showLogo) {
			dimage((128 - img_logo.width) / 2, 10, &img_logo);
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

		if (keydown(KEY_8)) {
			camera_moveInAriz(&camera, 0.5, camera.heading.ariz);
		}

		if (keydown(KEY_5)) {
			camera_moveInAriz(&camera, -0.5, camera.heading.ariz);
		}

		if (keydown(KEY_4)) {
			camera_moveInAriz(&camera, 0.5, camera.heading.ariz - 90);
		}

		if (keydown(KEY_6)) {
			camera_moveInAriz(&camera, 0.5, camera.heading.ariz + 90);
		}

		if (keydown(KEY_UP)) {
			camera.heading.incl -= 12;
		}

		if (keydown(KEY_DOWN)) {
			camera.heading.incl += 12;
		}

		if (keydown(KEY_LEFT)) {
			camera.heading.ariz -= 12;
		}

		if (keydown(KEY_RIGHT)) {
			camera.heading.ariz += 12;
		}
	}
}