#pragma once

#include <stdbool.h>
#include "../core/math.h"
#include "../ecs/ecs.h"

typedef struct {
    bool has_hit;
    Entity entity;
    Vec2 point;
    Vec2 normal;
    float distance;
} RaycastHit;

RaycastHit raycast_cast(Vec2 origin, Vec2 dir, float max_dist, Entity ignore_entity);
