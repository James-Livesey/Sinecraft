#ifndef UI_H_
#define UI_H_

#include <gint/display.h>

enum {
    INPUT_CHOICE_NONE,
    INPUT_CHOICE_CONFIRM,
    INPUT_CHOICE_EXIT,
    INPUT_CHOICE_MENU,
    INPUT_CHOICE_PREVIOUS,
    INPUT_CHOICE_NEXT,
    INPUT_CHOICE_FN
};

enum {
    MODIFIER_STATE_NONE,
    MODIFIER_STATE_SHIFT,
    MODIFIER_STATE_ALPHA,
    MODIFIER_STATE_ALPHA_LOCK
};

enum {
    CASING_STATE_UPPER,
    CASING_STATE_LOWER
};

void ui_button(int x1, int y1, int x2, int y2, char* text, bool selected);

void ui_input(int x1, int y1, int x2, int y2, char* text, unsigned int caretPosition, bool selected);
void ui_inputEvent(char* text, unsigned int* caretPosition, unsigned int maxLength, bool filenameFormatOnly, bool selected);

void ui_progressBar(int x1, int y1, int x2, int y2, double value);
void ui_progressLabel(int x1, int y1, int x2, int y2, char* text, bool selected);

void ui_functionIndicator(int fn, bopti_image_t* indicator);

void ui_message(char* line1, char* line2, char* line3, char* line4);

int ui_waitForInput(unsigned int* focus, unsigned int controlCount);
key_event_t ui_getKeyEvent();
int ui_getFnKey();

int ui_getModifierState();
void ui_setModifierState(int state);

int ui_getCasingState();
void ui_setCasingState(int state);
void ui_toggleCasingState();

#endif