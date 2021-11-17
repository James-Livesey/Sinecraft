#include <gint/gint.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/gray.h>
#include <gint/keyboard.h>
#include <gint/keycodes.h>

#include "common.h"
#include "camera.h"

extern bopti_image_t img_logo;

void main() {
	Camera camera = camera_default();

	dgray(DGRAY_ON);

	while (true) {
		dclear(C_WHITE);
		// dimage((128 - img_logo.width) / 2, 10, &img_logo);

		camera_render(camera);

		camera.heading.ariz += PI / 32;

		dupdate();

		key_event_t keyEvent = pollevent();

		if (keyEvent.key == KEY_MENU && keyEvent.type == KEYEV_DOWN) {
			gint_osmenu();
		}
	}
}