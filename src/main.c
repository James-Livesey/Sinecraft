#include <gint/gint.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>

extern bopti_image_t img_logo;

void main() {
	while (true) {
		dclear(C_WHITE);
		dimage((128 - img_logo.width) / 2, 10, &img_logo);
		dupdate();

		getkey();

		gint_osmenu();
	}
}