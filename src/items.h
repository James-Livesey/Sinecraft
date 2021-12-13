#ifndef ITEMS_H_
#define ITEMS_H_

#include <gint/display.h>
#include <gint/display-fx.h>

#define THUMBNAILS_COUNT 10

extern bopti_image_t* items_thumbnails[THUMBNAILS_COUNT];
extern unsigned int items_thumbnailMappings[THUMBNAILS_COUNT];

void items_init();

bool items_isPlaceable(int blockType);
char* items_getItemName(int blockType);
char* items_getItemNameShort(int blockType);

#endif