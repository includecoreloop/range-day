#include "weapon_ray.h"
#include "math.h"

WeaponRay weapon_calculate_ray(const Vec2 player_pos, const float rotation_deg, const float body_h,
                               const WeaponRender *weapon_render) {
    WeaponRay ray;

    const float gun_h = body_h * weapon_render->height_ratio;
    const Vec2 gun_local = {.x = weapon_render->x_position_offset, .y = body_h * weapon_render->y_position_offset};

    const float half_h = gun_h * 0.5f;

    const Vec2 front_local = {.x = gun_local.x, .y = gun_local.y - half_h};
    const Vec2 rear_local = {.x = gun_local.x, .y = gun_local.y + half_h};

    const float rad = rotation_deg * DEG2RAD;
    const float cos_a = cosf(rad);
    const float sin_a = sinf(rad);

    const Vec2 front_rot = {
        .x = cos_a * front_local.x - sin_a * front_local.y,
        .y = sin_a * front_local.x + cos_a * front_local.y
    };

    const Vec2 rear_rot = {
        .x = cos_a * rear_local.x - sin_a * rear_local.y,
        .y = sin_a * rear_local.x + cos_a * rear_local.y
    };

    ray.front = (Vec2){.x = player_pos.x + front_rot.x, .y = player_pos.y + front_rot.y};
    ray.rear = (Vec2){.x = player_pos.x + rear_rot.x, .y = player_pos.y + rear_rot.y};

    const Vec2 diff = {.x = ray.front.x - ray.rear.x, .y = ray.front.y - ray.rear.y};

    const float len = sqrtf(diff.x * diff.x + diff.y * diff.y);

    if (len > 0.0f) {
        ray.dir = (Vec2){.x = diff.x / len, .y = diff.y / len};
    } else {
        ray.dir = (Vec2){.x = 0.0f, .y = -1.0f};
    }

    return ray;
}
