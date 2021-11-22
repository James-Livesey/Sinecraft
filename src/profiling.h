#ifndef PROFILING_H
#define PROFILING_H

#include "flags.h"

#ifdef FLAG_PROFILING

#define PROFILING_SLOTS 16

typedef enum {
    PROFILING_RENDER_TIME = 0,
    PROFILING_WORLD_TO_CAMERA = 1,
    PROFILING_ORTH_TO_PERSP = 2,
    PROFILING_FIND_EDGES = 3,
    PROFILING_DRAW_FACES = 4
} ProfilingSlot;

void profiling_init();
void profiling_reset();
void profiling_start(ProfilingSlot slot);
void profiling_stop(ProfilingSlot slot);
unsigned int profiling_getTicks(ProfilingSlot slot);

#endif

#endif