#include <gint/gint.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>

#include "common.h"
#include "world.h"
#include "camera.h"

extern bopti_image_t img_logo;

void main() {
	World world = world_default();
	Camera camera = camera_default();

	camera.position.x = -4;
	camera.position.y = 0.5;

	world_addBlock(&world, (Block) {
		.position = (CartesianVector) {0, -0.5, 0}
	});

	world_addBlock(&world, (Block) {
		.position = (CartesianVector) {1, 0.5, 0}
	});

	world_addBlock(&world, (Block) {
		.position = (CartesianVector) {2, -0.5, 0}
	});

	while (true) {
		dclear(C_WHITE);

		camera_render(camera, world);
		dimage((128 - img_logo.width) / 2, 10, &img_logo);

		dupdate();

		clearevents();

		if (keydown(KEY_MENU) == 1) {
			gint_osmenu();
		}

		if (keydown(KEY_8)) {
			camera_moveInAriz(&camera, 0.5, camera.heading.ariz);
		}

		if (keydown(KEY_5)) {
			camera_moveInAriz(&camera, -0.5, camera.heading.ariz);
		}

		if (keydown(KEY_4)) {
			camera_moveInAriz(&camera, 0.5, camera.heading.ariz - (PI / 2));
		}

		if (keydown(KEY_6)) {
			camera_moveInAriz(&camera, 0.5, camera.heading.ariz + (PI / 2));
		}

		if (keydown(KEY_UP)) {
			camera.heading.incl -= PI / 16;
		}

		if (keydown(KEY_DOWN)) {
			camera.heading.incl += PI / 16;
		}

		if (keydown(KEY_LEFT)) {
			camera.heading.ariz -= PI / 16;
		}

		if (keydown(KEY_RIGHT)) {
			camera.heading.ariz += PI / 16;
		}
	}
}