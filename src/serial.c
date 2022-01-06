#include <stdbool.h>

#include "serial.h"

int serial_decodeInt(char* buffer, unsigned int* pointer) {
    serial_intConverter converter;

    for (unsigned int i = 0; i < 4; i++) {
        converter.asChars[i] = buffer[(*pointer)++];
    }

    return converter.asInt;
}

unsigned int serial_decodeUnsignedInt(char* buffer, unsigned int* pointer) {
    serial_intConverter converter;

    for (unsigned int i = 0; i < 4; i++) {
        converter.asChars[i] = buffer[(*pointer)++];
    }

    return converter.asUnsignedInt;
}

double serial_decodeDouble(char* buffer, unsigned int* pointer) {
    serial_doubleConverter converter;

    for (unsigned int i = 0; i < 8; i++) {
        converter.asChars[i] = buffer[(*pointer)++];
    }

    return converter.asDouble;
}

bool serial_decodeBool(char* buffer, unsigned int* pointer) {
    return (bool)buffer[(*pointer)++];
}

void serial_decodeCharArray(char* buffer, unsigned int* pointer, char* destination, unsigned int length) {
    for (unsigned int i = 0; i < length; i++) {
        destination[i] = buffer[(*pointer)++];
    }
}