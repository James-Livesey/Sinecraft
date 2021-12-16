#include <gint/display.h>
#include <gint/display-fx.h>

#include "common.h"

void ui_progressBar(int x1, int y1, int x2, int y2, double value) {
    if (value < 0) {
        value = 0;
    }

    if (value > 1) {
        value = 1;
    }

    drect_border(x1, y1, x2, y2, C_WHITE, 1, C_BLACK);

    drect(x1 + 2, y1 + 2, LERP(x1 + 2, x2 - 2, value), y2 - 2, C_BLACK);
}