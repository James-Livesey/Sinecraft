#ifndef UI_H_
#define UI_H_

enum {
    INPUT_CHOICE_NONE,
    INPUT_CHOICE_CONFIRM,
    INPUT_CHOICE_EXIT,
    INPUT_CHOICE_MENU
};

void ui_button(int x1, int y1, int x2, int y2, char* text, bool selected);
void ui_progressBar(int x1, int y1, int x2, int y2, double value);
void ui_progressLabel(int x1, int y1, int x2, int y2, char* text, bool selected);

int ui_waitForInput(unsigned int* focus, unsigned int controlCount);

#endif