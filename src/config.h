#ifndef CONFIG_H_
#define CONFIG_H_

typedef struct {
    unsigned int vernum;
    char username[8];
    int fov;
} Config;

Config config_default();

#endif