#include <stdbool.h>

#include "coords.h"

bool objects_hasClippedObject(CartesianVector point) {
    // TODO: Implement object/block memory; these are currently hard-coded
    // return (
    //     point.x >= 3 && point.x <= 4 &&
    //     point.y >= 0 && point.y <= 1 &&
    //     point.z >= 3 && point.z <= 4
    // ) || (
    //     point.x >= 4 && point.x <= 5 &&
    //     point.y >= 1 && point.y <= 2 &&
    //     point.z >= 3 && point.z <= 4
    // ) || (
    //     point.x >= 5 && point.x <= 6 &&
    //     point.y >= 0 && point.y <= 1 &&
    //     point.z >= 3 && point.z <= 4
    // );

    return (
        point.x >= 1 && point.x <= 2 &&
        point.y >= 1 && point.y <= 2 &&
        point.z >= 1 && point.z <= 2
    );
}

char objects_getClippedObjectColor(CartesianVector point) {
    return objects_hasClippedObject(point) ? 0x00 : 0xFF;
}