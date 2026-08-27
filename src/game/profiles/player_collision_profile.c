#include "player_collision_profile.h"

static const PlayerCollisionProfile PROFILE_DEFAULT = {
    .count = 3,
    .circles = {
        {.offset = {.x = 35.0f, .y = 0.0f}, .radius = 25.0f}, // Left shoulder/hip
        {.offset = {.x = -35.0f, .y = 0.0f}, .radius = 25.0f}, // Right shoulder/hip
        {.offset = {.x = 0.0f, .y = -10.0f}, .radius = 25.0f} // Center torso
    }
};

static const PlayerCollisionProfile PROFILE_AIM_PISTOL = {
    .count = 2,
    .circles = {
        {.offset = {.x = 0.0f, .y = -20.0f}, .radius = 55.0f}, // Torso & inner arms
        {.offset = {.x = 0.0f, .y = -100.0f}, .radius = 15.0f} // Pistol tip
    }
};

static const PlayerCollisionProfile PROFILE_AIM_RIFLE = {
    .count = 5,
    .circles = {
        {.offset = {.x = 0.0f, .y = -30.0f}, .radius = 40.0f}, // Torso
        {.offset = {.x = 12.0f, .y = -90.0f}, .radius = 20.0f}, // Handguard / Mid-barrel
        {.offset = {.x = 12.0f, .y = -120.0f}, .radius = 15.0f}, // Outer barrel
        {.offset = {.x = 12.0f, .y = -150.0f}, .radius = 15.0f}, // Muzzle tip
        {.offset = {.x = 25.0f, .y = 28.0f}, .radius = 22.0f} // Rear elbow/stock
    }
};

PlayerCollisionProfile player_collision_profile_get(const PlayerStanceState stance, const WeaponType weapon_type,
                                                    const bool has_weapon) {
    if (stance == PLAYER_STANCE_AIMING && has_weapon) {
        switch (weapon_type) {
            case WEAPON_PISTOL:
                return PROFILE_AIM_PISTOL;

            case WEAPON_RIFLE:
                return PROFILE_AIM_RIFLE;

            default:
                break;
        }
    }

    return PROFILE_DEFAULT;
}

void player_collision_profile_apply(Collider2D *col, const PlayerStanceState stance, const WeaponType weapon_type,
                                    const bool has_weapon) {
    col->type = COLLIDER_CIRCLE;

    const PlayerCollisionProfile profile = player_collision_profile_get(stance, weapon_type, has_weapon);

    col->circle_count = profile.count;

    for (int i = 0; i < profile.count; i++) {
        col->circles[i] = profile.circles[i];
    }
}
