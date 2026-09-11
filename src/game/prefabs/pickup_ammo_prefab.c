#include "pickup_ammo_prefab.h"
#include "../../components/transform.h"
#include "../../components/render.h"
#include "../../components/collider.h"
#include "../../components/size.h"
#include "../../components/pickup_ammo.h"

Entity pickup_ammo_prefab_create(const Vec2 position, const WeaponType weapon_type, const int ammo_count) {
    const Entity pickup = create_entity();

    const Transform2D pickup_transform = {
        .position = position,
        .scale = {.x = 1.0f, .y = 1.0f},
        .rotation = 0.0f
    };

    ADD_COMPONENT(transform, pickup, pickup_transform);

    const Render pickup_render = {
        .rgba = 0x00FF00FF
    };

    ADD_COMPONENT(render, pickup, pickup_render);

    const Size2D pickup_size = {
        .width = 30.0f,
        .height = 40.0f
    };

    ADD_COMPONENT(size, pickup, pickup_size);

    const Collider2D pickup_collider = {
        .type = COLLIDER_BOX,
        .is_static = true,
        .is_trigger = true,
        .half_extents = {.x = pickup_size.width * 0.5f, .y = pickup_size.height * 0.5f},
        .is_active = true,
    };

    ADD_COMPONENT(collider, pickup, pickup_collider);

    const PickupAmmo ammo_component = {
        .weapon_type = weapon_type,
        .ammo_count = ammo_count
    };

    ADD_COMPONENT(pickup_ammo, pickup, ammo_component);

    return pickup;
}

void pickup_ammo_prefab_destroy(const Entity entity) {
    REMOVE_COMPONENT(transform, entity);
    REMOVE_COMPONENT(render, entity);
    REMOVE_COMPONENT(size, entity);
    REMOVE_COMPONENT(collider, entity);
    REMOVE_COMPONENT(pickup_ammo, entity);

    destroy_entity(entity);
}
