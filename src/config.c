#include <gint/cpu.h>
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

    int file = BFile_Open(CONFIG_FILE_PATH, BFile_ReadOnly);

    if (file < 0) { // For example, file may not exist
        return config;
    }

    unsigned int fileSize = BFile_Size(file);
    unsigned int pointer = 0;

    char buffer[MAX_CONFIG_FILE_SIZE] = "";

    if (BFile_Read(file, &buffer, MIN(fileSize, MAX_CONFIG_FILE_SIZE), 0) < 0) {
        BFile_Close(file);

        return config; // Read error
    }

    BFile_Close(file);

    unsigned int vernum = serial_decodeUnsignedInt(buffer, &pointer);

    if (vernum > VERNUM) {
        return config; // Config is from newer version than expected
    }

    // Perform backwards-compatible conversion here

    config.vernum = vernum;

    serial_decodeCharArray(buffer, &pointer, config.username, sizeof(config.username));

    config.fov = common_rangeCheck(serial_decodeInt(buffer, &pointer), 30, 90, 60);
    config.camSpeed = common_rangeCheck(serial_decodeInt(buffer, &pointer), 0, 200, 100);
    config.invertY = serial_decodeBool(buffer, &pointer);

    return config;
}

int config_save(Config config) {
    cpu_atomic_start();

    int status;
    int size = sizeof(config);

    BFile_Remove(CONFIG_FILE_PATH);
    BFile_Create(CONFIG_FILE_PATH, BFile_File, &size);

    int file = BFile_Open(CONFIG_FILE_PATH, BFile_WriteOnly);

    if (file < 0) {
        cpu_atomic_end();

        return file; // Something went wrong
    }

    status = BFile_Write(file, &config, size);

    BFile_Close(file);

    cpu_atomic_end();

    return status;
}