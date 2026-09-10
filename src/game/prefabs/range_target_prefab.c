#include "range_target_prefab.h"
#include "../../components/range_target.h"
#include "../../components/transform.h"
#include "../../components/render.h"
#include "../../components/collider.h"
#include "../../components/size.h"


Entity range_target_prefab_create(const Vec2 position, const Size2D target_size, const RangeTargetType target_type,
                                  const float spawn_delay, const float active_duration) {
    const Entity rt = create_entity();

    const Transform2D target_transform = {
        .position = position,
        .scale = {.x = 1.0f, .y = 1.0f},
        .rotation = 0.0f
    };

    ADD_COMPONENT(transform, rt, target_transform);

    const Render target_render = {
        .rgba = 0x0000FFFF
    };

    ADD_COMPONENT(render, rt, target_render);

    ADD_COMPONENT(size, rt, target_size);

    const RangeTarget rt_component = {
        .state = RANGE_TARGET_STATE_PENDING,
        .type = target_type,
        .spawn_delay = spawn_delay,
        .active_duration = active_duration,
    };

    ADD_COMPONENT(range_target, rt, rt_component);

    const Collider2D target_collider = {
        .type = COLLIDER_BOX,
        .is_static = true,
        .is_trigger = false,
        .half_extents = {.x = target_size.width * 0.5, .y = target_size.height * 0.5},
        .is_active = false,
    };

    ADD_COMPONENT(collider, rt, target_collider);

    return rt;
}

void range_target_prefab_destroy(const Entity entity) {
    REMOVE_COMPONENT(transform, entity);
    REMOVE_COMPONENT(render, entity);
    REMOVE_COMPONENT(size, entity);
    REMOVE_COMPONENT(collider, entity);
    REMOVE_COMPONENT(range_target, entity);
    destroy_entity(entity);
}
