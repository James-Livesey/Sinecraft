#include <string.h>
#include <gint/display.h>
#include <gint/display-fx.h>
#include <gint/keyboard.h>

#include "ui.h"
#include "common.h"
#include "keys.h"

key_event_t lastKeyEvent;
int lastFnKey = 0;
int modifierState = MODIFIER_STATE_NONE; // The modifier state that factors next key, should be read by external code
int immediateModifierState = MODIFIER_STATE_NONE; // The immediate modifier state that resets after next key is pressed
int internalModifierState = MODIFIER_STATE_NONE; // The modifier state that resets immediately after modifier key is pressed

void ui_button(int x1, int y1, int x2, int y2, char* text, bool selected) {
    dline(x1 + 1, y1, x2 - 1, y1, C_BLACK);
    dline(x1 + 1, y2, x2 - 1, y2, C_BLACK);
    dline(x1, y1 + 1, x1, y2 - 1, C_BLACK);
    dline(x2, y1 + 1, x2, y2 - 1, C_BLACK);

    drect(x1 + 1, y1 + 1, x2 - 1, y2 - 1, selected ? C_BLACK : C_WHITE);

    dtext_opt((x1 + x2) / 2, (y1 + y2) / 2, C_INVERT, C_NONE, DTEXT_CENTER, DTEXT_CENTER, text, -1);
}

void ui_input(int x1, int y1, int x2, int y2, char* text, unsigned int caretPosition, bool selected) {
    drect_border(x1, y1, x2, y2, C_WHITE, 1, C_BLACK);

    dtext_opt(x1 + 2, (y1 + y2) / 2, C_BLACK, C_NONE, DTEXT_LEFT, DTEXT_CENTER, text, ((x2 - x1) / 6) - 6);

    if (!selected) {
        return;
    }

    int caretStart = x1 + 2 + (caretPosition * 6);

    drect(caretStart, ((y1 + y2) / 2) - 4, caretStart + 1, ((y1 + y2) / 2) + 4, C_INVERT);
}

// `maxLength` should be 1 less than the actual string length to allow for null termination
void ui_inputEvent(char* text, unsigned int* caretPosition, unsigned int maxLength, bool filenameFormatOnly, bool selected) {
    if (!selected) {
        return;
    }

    switch (lastKeyEvent.key) {
        case KEY_LEFT:
            if (*caretPosition == 0) {
                *caretPosition = strlen(text);

                break;
            }

            (*caretPosition)--;

            break;

        case KEY_RIGHT:
            if (*caretPosition >= strlen(text)) {
                *caretPosition = 0;

                break;
            }

            (*caretPosition)++;

            break;

        case KEY_DEL:
            if (*caretPosition == 0) { // Deletion from start
                (*caretPosition)++;
            }

            for (unsigned int i = *caretPosition - 1; i < maxLength; i++) {
                text[i] = text[i + 1];
            }

            (*caretPosition)--;

            break;

        default:
            if (*caretPosition == maxLength) {
                break;
            }

            char* string = keys_getString(
                lastKeyEvent.key,
                modifierState == MODIFIER_STATE_SHIFT,
                modifierState == MODIFIER_STATE_ALPHA || modifierState == MODIFIER_STATE_ALPHA_LOCK,
                filenameFormatOnly
            );

            if (strlen(string) == 0) {
                break;
            }

            for (unsigned int i = 0; i < strlen(string); i++) {
                for (unsigned int j = maxLength - 2; j > *caretPosition; j--) {
                    text[j + 1] = text[j];
                }

                text[*caretPosition + 1] = text[*caretPosition];
                text[maxLength] = 0; // Ensure null pointer at end of text

                text[*caretPosition] = string[i];

                (*caretPosition)++;
            }

            break;
    }
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

void ui_message(char* line1, char* line2, char* line3, char* line4) {
    drect(13, 13, 116, 52, C_BLACK);
    drect_border(12, 12, 115, 51, C_WHITE, 1, C_BLACK);

    dtext(16, 16, C_BLACK, line1);
    dtext(16, 24, C_BLACK, line2);
    dtext(16, 32, C_BLACK, line3);
    dtext(16, 40, C_BLACK, line4);
}

int ui_waitForInput(unsigned int* focus, unsigned int controlCount) {
    key_event_t keyEvent = getkey_opt(GETKEY_DEFAULT & ~GETKEY_MENU & ~GETKEY_MOD_SHIFT & ~GETKEY_MOD_ALPHA, NULL);
    int choice = INPUT_CHOICE_NONE;

    lastKeyEvent = keyEvent;

    switch (keyEvent.key) {
        case KEY_SHIFT:
            internalModifierState = MODIFIER_STATE_SHIFT;
            break;

        case KEY_ALPHA:
            if (immediateModifierState == MODIFIER_STATE_ALPHA_LOCK) {
                internalModifierState = MODIFIER_STATE_NONE;

                break;
            }

            if (immediateModifierState == MODIFIER_STATE_SHIFT) {
                internalModifierState = MODIFIER_STATE_ALPHA_LOCK;

                break;
            }

            internalModifierState = MODIFIER_STATE_ALPHA;

            break;

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
            choice = INPUT_CHOICE_PREVIOUS;
            break;

        case KEY_RIGHT:
            choice = INPUT_CHOICE_NEXT;
            break;

        case KEY_EXE:
            choice = INPUT_CHOICE_CONFIRM;
            break;

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

            choice = INPUT_CHOICE_FN;
            break;

        case KEY_EXIT:
            choice = INPUT_CHOICE_EXIT;
            break;

        case KEY_MENU:
            choice = INPUT_CHOICE_MENU;
            break;
    }

    modifierState = immediateModifierState;
    immediateModifierState = internalModifierState;

    if ((immediateModifierState == MODIFIER_STATE_SHIFT && keyEvent.key != KEY_ALPHA) || immediateModifierState == MODIFIER_STATE_ALPHA) {
        internalModifierState = MODIFIER_STATE_NONE;
    }

    return choice;
}

key_event_t ui_getKeyEvent() {
    return lastKeyEvent;
}

int ui_getFnKey() {
    return lastFnKey;
}

int ui_getModifierState() {
    return modifierState;
}

void ui_setModifierState(int state) {
    modifierState = state;
    immediateModifierState = state;
    internalModifierState = state;
}