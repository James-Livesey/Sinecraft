#include "keys.h"

#define ksm KeyStringMap

const KeyStringMap KEY_STRINGS[] = {
    (ksm) {KEY_7, "7"}, (ksm) {KEY_8, "8"}, (ksm) {KEY_9, "9"},
    (ksm) {KEY_4, "4"}, (ksm) {KEY_5, "5"}, (ksm) {KEY_6, "6"},
    (ksm) {KEY_1, "1"}, (ksm) {KEY_2, "2"}, (ksm) {KEY_3, "3"},
    (ksm) {KEY_0, "0"}, (ksm) {KEY_DOT, "."}, (ksm) {KEY_NEG, "-"},
    (ksm) {-1, ""}
};

const KeyStringMap KEY_STRINGS_ALPHA[] = {
    (ksm) {KEY_SQUARE, "r"}, (ksm) {KEY_POWER, "\xE6\x47"},
    (ksm) {KEY_XOT, "A"}, (ksm) {KEY_LOG, "B"}, (ksm) {KEY_LN, "C"}, (ksm) {KEY_SIN, "D"}, (ksm) {KEY_COS, "E"}, (ksm) {KEY_TAN, "F"},
    (ksm) {KEY_FRAC, "G"}, (ksm) {KEY_FD, "H"}, (ksm) {KEY_LEFTP, "I"}, (ksm) {KEY_RIGHTP, "J"}, (ksm) {KEY_COMMA, "K"}, (ksm) {KEY_ARROW, "L"},
    (ksm) {KEY_7, "M"}, (ksm) {KEY_8, "N"}, (ksm) {KEY_9, "O"},
    (ksm) {KEY_4, "P"}, (ksm) {KEY_5, "Q"}, (ksm) {KEY_6, "R"}, (ksm) {KEY_MUL, "S"}, (ksm) {KEY_DIV, "T"},
    (ksm) {KEY_1, "U"}, (ksm) {KEY_2, "V"}, (ksm) {KEY_3, "W"}, (ksm) {KEY_ADD, "X"}, (ksm) {KEY_SUB, "Y"},
    (ksm) {KEY_0, "Z"}, (ksm) {KEY_DOT, " "}, (ksm) {KEY_EXP, "\""},
    (ksm) {-1, ""}
};

char* keys_getString(int keyCode, bool alpha, bool filenameFormatOnly) {
    const KeyStringMap* map = alpha ? KEY_STRINGS_ALPHA : KEY_STRINGS;

    for (unsigned int i = 0; map[i].keyCode > 0; i++) {
        if (filenameFormatOnly) {
            if (alpha && (keyCode == KEY_SQUARE || keyCode == KEY_POWER || keyCode == KEY_EXP)) {
                return "";
            }

            if (keyCode == KEY_DOT || keyCode == KEY_NEG) {
                return "";
            }
        }

        if (map[i].keyCode == keyCode) {
            return map[i].string;
        }
    }

    return "";
}