#ifndef PHYSICS_H_
#define PHYSICS_H_

#include "common.h"
#include "coords.h"
#include "world.h"
#include "camera.h"

#define GRAVITATIONAL_FIELD_STRENGTH 100

typedef struct {
    Camera* camera;
    World* world;
    double yVelocity;
    double yAcceleration;
} PhysicsSimulation;

int physics_tick();
unsigned int physics_getCurrentTime();

void physics_init();
PhysicsSimulation physics_default(Camera* camera, World* world);
void physics_update(PhysicsSimulation* sim);
void physics_updateDelta();
void physics_jump(PhysicsSimulation* sim);

#endif