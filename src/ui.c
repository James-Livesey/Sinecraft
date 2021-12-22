#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>

#include "ui.h"
#include "common.h"

void ui_button(int x1, int y1, int x2, int y2, char* text, bool selected) {
    dline(x1 + 1, y1, x2 - 1, y1, C_BLACK);
    dline(x1 + 1, y2, x2 - 1, y2, C_BLACK);
    dline(x1, y1 + 1, x1, y2 - 1, C_BLACK);
    dline(x2, y1 + 1, x2, y2 - 1, C_BLACK);

    drect(x1 + 1, y1 + 1, x2 - 1, y2 - 1, selected ? C_BLACK : C_WHITE);

    dtext_opt((x1 + x2) / 2, (y1 + y2) / 2, C_INVERT, C_NONE, DTEXT_CENTER, DTEXT_CENTER, text, -1);
}

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

void ui_progressLabel(int x1, int y1, int x2, int y2, char* text, bool selected) {
    if (selected) {
        drect(x1 + 1, y1 + 1, x2 - 1, y2 - 1, C_INVERT);
    }

    dtext_opt((x1 + x2) / 2, (y1 + y2) / 2, C_INVERT, C_NONE, DTEXT_CENTER, DTEXT_CENTER, text, -1);
}

int ui_waitForInput(unsigned int* focus, unsigned int controlCount) {
    key_event_t keyEvent = getkey();

    switch (keyEvent.key) {
        case KEY_UP:
        case KEY_LEFT:
            if (*focus == 0) {
                *focus = controlCount - 1;

                break;
            }

            (*focus)--;

            break;

        case KEY_DOWN:
        case KEY_RIGHT:
            (*focus)++;

            if (*focus >= controlCount) {
                *focus = 0;
            }

            break;

        case KEY_EXE:
            return INPUT_CHOICE_CONFIRM;

        case KEY_EXIT:
            return INPUT_CHOICE_EXIT;

        case KEY_MENU:
            return INPUT_CHOICE_MENU;
    }

    return INPUT_CHOICE_NONE;
}