#include <gint/bfile.h>

#include "config.h"
#include "flags.h"
#include "common.h"
#include "serial.h"

Config config_default() {
    return (Config) {
        .vernum = VERNUM,
        .username = "Steve",
        .fov = 60,
        .camSpeed = 100,
        .invertY = false
    };
}

Config config_load() {
    Config config = config_default();

    int file = BFile_Open(u"Sincft.cfg", BFile_ReadOnly);

    if (file < 0) { // For example, file may not exist
        return config;
    }

    unsigned int fileSize = BFile_Size(file);
    unsigned int pointer = 0;

    char buffer[MAX_CONFIG_FILE_SIZE] = "";

    BFile_Read(file, &buffer, MIN(fileSize, MAX_CONFIG_FILE_SIZE), 0);

    unsigned int vernum = serial_decodeUnsignedInt(buffer, &pointer);

    if (vernum > VERNUM) {
        return config; // Config is from newer version than expected
    }

    // Perform backwards-compatible conversion here

    config.vernum = vernum;

    serial_decodeCharArray(buffer, &pointer, config.username, sizeof(config.username));

    config.fov = serial_decodeInt(buffer, &pointer);
    config.camSpeed = serial_decodeInt(buffer, &pointer);
    config.invertY = serial_decodeBool(buffer, &pointer);

    return config;
}