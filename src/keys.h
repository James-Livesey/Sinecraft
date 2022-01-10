#ifndef KEYS_H_
#define KEYS_H_

#include <gint/keyboard.h>

typedef struct {
    int keyCode;
    char* string;
} KeyStringMap;

char* keys_getString(int keyCode, bool alpha, bool filenameFormatOnly);

#endif