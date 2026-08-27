#pragma once

#include "../ecs/ecs.h"
#include "../core/math.h"

typedef struct {
    Vec2 position;
    Vec2 scale;
    float rotation;
} Transform2D;

COMPONENT_DEFINE(Transform2D, transform);
