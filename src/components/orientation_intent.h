#pragma once

#include "../ecs/ecs.h"
#include "../core/math.h"
#include <stdbool.h>

typedef struct {
    Vec2 look_at_pos;
    bool look_at_moving_direction;
} OrientationIntent;

COMPONENT_DEFINE(OrientationIntent, orientation_intent);
