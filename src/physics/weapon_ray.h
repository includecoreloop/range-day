#pragma once

#include "../core/math.h"
#include "../components/inventory.h"

typedef struct {
    Vec2 rear;
    Vec2 front;
    Vec2 dir;
} WeaponRay;

WeaponRay weapon_calculate_ray(Vec2 player_pos, float rotation_deg, float body_h, const WeaponRender *weapon_render);
