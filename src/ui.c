#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>

#include "ui.h"
#include "common.h"

int lastFnKey = 0;

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

void ui_functionIndicator(int fn, bopti_image_t* indicator) {
    dimage(2 + ((fn - 1) * 21), 56, indicator);
}

int ui_waitForInput(unsigned int* focus, unsigned int controlCount) {
    key_event_t keyEvent = getkey_opt(GETKEY_DEFAULT & ~GETKEY_MENU, NULL);

    switch (keyEvent.key) {
        case KEY_UP:
            if (*focus == 0) {
                *focus = controlCount - 1;

                break;
            }

            (*focus)--;

            break;

        case KEY_DOWN:
            (*focus)++;

            if (*focus >= controlCount) {
                *focus = 0;
            }

            break;

        case KEY_LEFT:
            return INPUT_CHOICE_PREVIOUS;

        case KEY_RIGHT:
            return INPUT_CHOICE_NEXT;

        case KEY_EXE:
            return INPUT_CHOICE_CONFIRM;

        case KEY_F1:
        case KEY_F2:
        case KEY_F3:
        case KEY_F4:
        case KEY_F5:
        case KEY_F6:
            lastFnKey = keycode_function(keyEvent.key);

            if (lastFnKey < 0) {
                lastFnKey = 0;
            }

            return INPUT_CHOICE_FN;

        case KEY_EXIT:
            return INPUT_CHOICE_EXIT;

        case KEY_MENU:
            return INPUT_CHOICE_MENU;
    }

    return INPUT_CHOICE_NONE;
}

int ui_getFnKey() {
    return lastFnKey;
}