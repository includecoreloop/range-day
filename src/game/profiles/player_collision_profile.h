#pragma once

#include <stdbool.h>
#include "../../components/collider.h"
#include "../../components/player.h"
#include "../../components/inventory.h"

#include "../../core/config.h"

typedef struct {
    CircleShape circles[MAX_COLLIDER_CIRCLES];
    int count;
} PlayerCollisionProfile;

PlayerCollisionProfile player_collision_profile_get(PlayerStanceState stance, WeaponType weapon_type, bool has_weapon);

void player_collision_profile_apply(Collider2D *col, PlayerStanceState stance, WeaponType weapon_type, bool has_weapon);
