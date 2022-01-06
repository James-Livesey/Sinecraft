#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdbool.h>

typedef union {
    char asChars[4];
    int asInt;
    unsigned int asUnsignedInt;
} serial_intConverter;

typedef union {
    char asChars[8];
    double asDouble;
} serial_doubleConverter;

int serial_decodeInt(char* buffer, unsigned int* pointer);
unsigned int serial_decodeUnsignedInt(char* buffer, unsigned int* pointer);
double serial_decodeDouble(char* buffer, unsigned int* pointer);
bool serial_decodeBool(char* buffer, unsigned int* pointer);
void serial_decodeCharArray(char* buffer, unsigned int* pointer, char* destination, unsigned int length);

#endif