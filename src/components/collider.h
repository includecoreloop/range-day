#pragma once

#include "../ecs/ecs.h"
#include "../core/math.h"
#include <stdbool.h>

#include "../core/config.h"

typedef enum {
    COLLIDER_CIRCLE,
    COLLIDER_BOX
} ColliderType;

typedef struct {
    Vec2 offset;
    float radius;
} CircleShape;

typedef struct {
    ColliderType type;
    bool is_static;
    bool is_trigger;

    Vec2 offset;
    Vec2 half_extents;

    CircleShape circles[MAX_COLLIDER_CIRCLES];
    int circle_count;

    bool is_active;
} Collider2D;

COMPONENT_DEFINE(Collider2D, collider);
