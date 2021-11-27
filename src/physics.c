#include <math.h>
#include <gint/timer.h>

#include "physics.h"
#include "common.h"
#include "coords.h"

unsigned int currentTime = 0;
unsigned int lastTime = 0;

int physics_tick() {
    currentTime++;

    return TIMER_CONTINUE;
}

void physics_init() {
    lastTime = 0;

    int timer = timer_configure(TIMER_ETMU, 1e4, GINT_CALL(physics_tick));

    timer_start(timer);
}

PhysicsSimulation physics_default(Camera* camera, World* world) {
    return (PhysicsSimulation) {
        .camera = camera,
        .world = world,
        .yVelocity = 0,
        .yAcceleration = 0
    };
}

void physics_update(PhysicsSimulation* sim) {
    double delta = currentTime - lastTime;
    double initialVelocity = sim->yVelocity;

    CartesianVector initialPosition = sim->camera->position;

    // s = ut - (1/2)a(t^2)
    double distanceToMove = (initialVelocity * (delta / 1000)) - (0.5 * sim->yAcceleration * SQUARE(delta / 1000));

    sim->camera->position = coords_addCartesian(sim->camera->position, (CartesianVector) {0, distanceToMove, 0});

    // v = u + at
    sim->yVelocity = initialVelocity + (sim->yAcceleration * (delta / 1000));

    CartesianVector positionBelow = (CartesianVector) {
        round(sim->camera->position.x),
        round(sim->camera->position.y - 2),
        round(sim->camera->position.z)
    };
    Block blockBelow = world_getBlock(*(sim->world), positionBelow);

    if (sim->camera->position.y < 1 || blockBelow.type != BLOCK_TYPE_AIR) {
        sim->yVelocity = 0;
        sim->yAcceleration = 0;

        sim->camera->position = initialPosition;
    } else {
        sim->yAcceleration = -GRAVITATIONAL_FIELD_STRENGTH;
    }
}

void physics_updateDelta() {
    lastTime = currentTime;
}

void physics_jump(PhysicsSimulation* sim) {
    CartesianVector positionBelow = (CartesianVector) {
        round(sim->camera->position.x),
        round(sim->camera->position.y - 2),
        round(sim->camera->position.z)
    };
    Block blockBelow = world_getBlock(*(sim->world), positionBelow);

    if (sim->camera->position.y > 1.1 && blockBelow.type == BLOCK_TYPE_AIR) {
        return;
    }

    // v = sqrt((u^2) + 2as)
    sim->yVelocity = sqrt(2 * GRAVITATIONAL_FIELD_STRENGTH * 1.5);
}