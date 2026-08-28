#include "obstacle_prefab.h"
#include "../../components/transform.h"
#include "../../components/render.h"
#include "../../components/collider.h"
#include "../../components/size.h"

Entity obstacle_prefab_create(const Vec2 position, const Size2D obstacle_size) {
    const Entity obstacle = create_entity();

    const Transform2D obstacle_transform = {
        .position = position,
        .scale = {.x = 1.0f, .y = 1.0f},
        .rotation = 0.0f
    };

    ADD_COMPONENT(transform, obstacle, obstacle_transform);

    const Render obstacle_render = {
        .rgba = 0x0000FFFF
    };

    ADD_COMPONENT(render, obstacle, obstacle_render);

    ADD_COMPONENT(size, obstacle, obstacle_size);

    const Collider2D obstacle_collider = {
        .type = COLLIDER_BOX,
        .is_static = true,
        .is_trigger = false,
        .half_extents = {.x = obstacle_size.width * 0.5, .y = obstacle_size.height * 0.5},
        .is_active = true,
    };

    ADD_COMPONENT(collider, obstacle, obstacle_collider);

    return obstacle;
}

void obstacle_prefab_destroy(const Entity obstacle) {
    REMOVE_COMPONENT(transform, obstacle);
    REMOVE_COMPONENT(render, obstacle);
    REMOVE_COMPONENT(size, obstacle);
    REMOVE_COMPONENT(collider, obstacle);
    destroy_entity(obstacle);
}
