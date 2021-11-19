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
		dimage((128 - img_logo.width) / 2, 10, &img_logo);

		camera_render(camera);

		dupdate();

		key_event_t keyEvent = pollevent();

		if (keyEvent.type == KEYEV_DOWN) {
			switch (keyEvent.key) {
				case KEY_MENU:
					gint_osmenu();
					break;

				case KEY_8:
					camera_moveInAriz(&camera, 0.5, camera.heading.ariz);
					break;

				case KEY_UP:
					camera.heading.incl -= PI / 16;
					break;

				case KEY_DOWN:
					camera.heading.incl += PI / 16;
					break;

				case KEY_LEFT:
					camera.heading.ariz -= PI / 16;
					break;

				case KEY_RIGHT:
					camera.heading.ariz += PI / 16;
					break;
			}
		}
	}
}