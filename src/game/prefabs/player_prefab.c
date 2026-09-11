#include "player_prefab.h"
#include "../../components/transform.h"
#include "../../components/render.h"
#include "../../components/humanoid_render.h"
#include "../../components/velocity.h"
#include "../../components/player.h"
#include "../../components/orientation_intent.h"
#include "../../components/collider.h"
#include "../../components/inventory.h"
#include "../../components/size.h"

#include "../profiles/player_collision_profile.h"

Entity player_prefab_create(void) {
    const Entity player_entity = create_entity();

    const Transform2D player_transform = {
        .position = {.x = 640.0f, .y = 360.0f},
        .scale = {.x = 1.0f, .y = 1.0f},
        .rotation = 0.0f
    };

    ADD_COMPONENT(transform, player_entity, player_transform);

    const Render player_render = {
        .rgba = 0x0000FFFF
    };

    ADD_COMPONENT(render, player_entity, player_render);

    const Size2D player_size = {
        .width = 110.0f,
        .height = 45.0f,
    };

    ADD_COMPONENT(size, player_entity, player_size);

    const HumanoidRender player_humanoid_render = {
        .torso_corner_radius = 0.12f,
        .head_width_ratio = 0.45f,
        .head_height_ratio = 1.50f,
        .head_forward_offset = -0.15f,

        .foot_width_ratio = 0.22f,
        .foot_height_ratio = 0.70f,
        .foot_lateral_offset = 0.25f,
        .foot_corner_radius = 0.30f,

        .arm_corner_radius = 0.3f,

        .pistol_arm_width_ratio = 0.22f,
        .pistol_arm_height_ratio = 1.85f,
        .pistol_shoulder_x_ratio = 0.55f,
        .pistol_shoulder_y_offset = -1.3f,
        .pistol_grip_y_offset = -0.55f,
        .pistol_arm_aim_angle = 28.0f,

        .rifle_aim_torso_angle = 45.0f,

        .rifle_arm_width_ratio = 0.22f,
        .rifle_arm_height_ratio = 1.85f,
        .rifle_shoulder_x_ratio = 0.55f,
        .rifle_l_shoulder_y_offset = -1.3f,
        .rifle_r_shoulder_y_offset = -0.5f,
        .rifle_grip_y_offset = -0.55f,
        .rifle_l_arm_aim_angle = 34.0f,
        .rifle_r_arm_aim_angle = 10.0f,

        .rifle_l_arm_midpoint_x_scale = 0.3f,
        .rifle_l_arm_midpoint_y_scale = 1.6f,

        .walk_stride_amplitude = 0.30f,
        .run_stride_amplitude = 0.45f,
        .walk_stride_speed = 7.0f,
        .run_stride_speed = 10.5f,
        .leg_phase = 0.0f,
        .current_stride_offset = 0.0f
    };

    ADD_COMPONENT(humanoid_render, player_entity, player_humanoid_render);

    const Velocity player_velocity = {
        .direction = {.x = 0.0f, .y = 0.0f},
        .speed = 0.0f
    };

    ADD_COMPONENT(velocity, player_entity, player_velocity);

    const Player player_component = {
        .locomotion_state = PLAYER_LOCOMOTION_IDLE,
        .stance_state = PLAYER_STANCE_NORMAL,
        .action_state = PLAYER_ACTION_DEFAULT,
        .run_speed = 300.0f,
        .walk_speed = 200.0f,
        .cqb_aim_speed = 75.0f
    };

    ADD_COMPONENT(player, player_entity, player_component);

    const OrientationIntent orientation_intent_component = {
        .look_at_pos = {.x = 0.0f, .y = 0.0f},
        .look_at_moving_direction = true,
    };

    ADD_COMPONENT(orientation_intent, player_entity, orientation_intent_component);

    Collider2D player_collider = {
        .type = COLLIDER_CIRCLE,
        .is_static = false,
        .is_trigger = false,
        .is_active = true,
    };

    player_collision_profile_apply(&player_collider, PLAYER_STANCE_NORMAL, (WeaponType) 0, false);

    ADD_COMPONENT(collider, player_entity, player_collider);

    Inventory player_inv = {0};

    player_inv.weapons[WEAPON_PISTOL] = (Weapon){
        .type = WEAPON_PISTOL,
        .fire_rate = 0.25f,
        .ammo_in_mag = 12,
        .mag_size = 12,
        .reserve_ammo = 0,
        .max_reserve_ammo = 24,
        .render = {
            .width_ratio = 0.20f,
            .height_ratio = 1.20f,
            .corner_radius = 0.15f,
            .x_position_offset = 0.0f,
            .y_position_offset = -2.0f,
            .muzzle_flash_length = 24.0f,
            .muzzle_flash_width = 14.0f,
            .muzzle_flash_duration = 0.08f,
            .recoil_kick_distance = 6.0f,
            .recoil_duration = 0.08f,
        },
        .reload_duration = 1.0f,
    };

    player_inv.weapons[WEAPON_RIFLE] = (Weapon){
        .type = WEAPON_RIFLE,
        .fire_rate = 0.1f,
        .ammo_in_mag = 25,
        .mag_size = 25,
        .reserve_ammo = 0,
        .max_reserve_ammo = 50,
        .render = {
            .width_ratio = 0.20f,
            .height_ratio = 3.20f,
            .corner_radius = 0.15f,
            .x_position_offset = 12.0f,
            .y_position_offset = -2.0f,
            .muzzle_flash_length = 34.0f,
            .muzzle_flash_width = 18.0f,
            .muzzle_flash_duration = 0.08f,
            .recoil_kick_distance = 6.0f,
            .recoil_duration = 0.08f,
        },
        .reload_duration = 1.5f,
    };

    player_inv.active_index = WEAPON_PISTOL;

    ADD_COMPONENT(inventory, player_entity, player_inv);

    return player_entity;
}

void player_prefab_destroy(const Entity entity) {
    REMOVE_COMPONENT(transform, entity);
    REMOVE_COMPONENT(render, entity);
    REMOVE_COMPONENT(size, entity);
    REMOVE_COMPONENT(humanoid_render, entity);
    REMOVE_COMPONENT(velocity, entity);
    REMOVE_COMPONENT(collider, entity);
    REMOVE_COMPONENT(player, entity);
    REMOVE_COMPONENT(orientation_intent, entity);
    REMOVE_COMPONENT(inventory, entity);

    destroy_entity(entity);
}
