#pragma once
#include "../core/math.h"
#include "../ecs/ecs.h"

typedef struct {
    Vec2 direction;
    float speed;
} Velocity;

COMPONENT_DEFINE(Velocity, velocity);
