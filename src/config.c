#include "config.h"
#include "flags.h"

Config config_default() {
    return (Config) {
        .vernum = VERNUM,
        .username = "Steve",
        .fov = 60,
        .camSpeed = 100
    };
}