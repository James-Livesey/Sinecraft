#include <stdbool.h>

#include "serial.h"

int serial_decodeInt(char* buffer, unsigned int* pointer) {
    serial_intConverter value;

    for (unsigned int i = 0; i < 4; i++) {
        value.asChars[i] = buffer[(*pointer)++];
    }

    return value.asInt;
}

unsigned int serial_decodeUnsignedInt(char* buffer, unsigned int* pointer) {
    serial_intConverter value;

    for (unsigned int i = 0; i < 4; i++) {
        value.asChars[i] = buffer[(*pointer)++];
    }

    return value.asUnsignedInt;
}

bool serial_decodeBool(char* buffer, unsigned int* pointer) {
    return (bool)buffer[(*pointer)++];
}

void serial_decodeCharArray(char* buffer, unsigned int* pointer, char* destination, unsigned int length) {
    for (unsigned int i = 0; i < length; i++) {
        destination[i] = buffer[(*pointer)++];
    }
}