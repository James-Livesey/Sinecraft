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

void ui_button(int x1, int y1, int x2, int y2, char* text, bool selected);
void ui_progressBar(int x1, int y1, int x2, int y2, double value);
void ui_progressLabel(int x1, int y1, int x2, int y2, char* text, bool selected);
void ui_functionIndicator(int fn, bopti_image_t* indicator);

int ui_waitForInput(unsigned int* focus, unsigned int controlCount);
int ui_getFnKey();

#endif