#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdbool.h>

typedef struct {
    unsigned int vernum;
    char username[8];
    int fov;
    int camSpeed;
    bool invertY;
} Config;

Config config_default();

#endif