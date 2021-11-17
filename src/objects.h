#ifndef OBJECTS_H_
#define OBJECTS_H_

#include <stdbool.h>

#include "coords.h"

bool objects_hasClippedObject(CartesianVector point);
char objects_getClippedObjectColor(CartesianVector point);

#endif