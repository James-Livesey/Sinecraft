#ifndef CONFIG_H_
#define CONFIG_H_

#include <stdbool.h>

#define MAX_CONFIG_FILE_SIZE 128

typedef struct {
    unsigned int vernum;
    char username[8];
    int fov;
    int camSpeed;
    bool invertY;
} Config;

Config config_default();
Config config_load();

#endif