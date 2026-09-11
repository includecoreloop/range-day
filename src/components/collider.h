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

    Entity triggered_entities[MAX_TRIGGER_CONTACTS];
    int trigger_contact_count;
} Collider2D;

COMPONENT_DEFINE(Collider2D, collider);

static void collider_add_trigger_contact(Collider2D *col, const Entity other_entity) {
    col->triggered_entities[col->trigger_contact_count] = other_entity;
    col->trigger_contact_count++;
}
