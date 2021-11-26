#include "flags.h"
#include "profiling.h"

#ifdef FLAG_PROFILING

#include <gint/gint.h>
#include <gint/timer.h>

uint32_t ticks = 0;
uint32_t startTicks[16];
unsigned int recordedTicks[16];

int profiling_tick() {
    ticks++;

    return TIMER_CONTINUE;
}

void profiling_init() {
    int timer = timer_configure(TIMER_ETMU, 100, GINT_CALL(profiling_tick));

    timer_start(timer);
}

void profiling_reset() {
    for (unsigned int i = 0; i < PROFILING_SLOTS; i++) {
        startTicks[i] = 0;
        recordedTicks[i] = 0;
    }
}

void profiling_start(ProfilingSlot slot) {
    startTicks[slot] = ticks;
}

void profiling_stop(ProfilingSlot slot) {
    recordedTicks[slot] = ticks - startTicks[slot];
}

unsigned int profiling_getTicks(ProfilingSlot slot) {
    return recordedTicks[slot];
}

#endif