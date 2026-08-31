#include "combat_system.h"

#include "../components/player.h"
#include "../components/collider.h"
#include "../components/inventory.h"
#include "../components/range_target.h"
#include "../components/transform.h"
#include "../components/size.h"
#include "../core/audio_bridge.h"
#include "../physics/raycast.h"
#include "../physics/weapon_ray.h"

#include "../game/prefabs/particle_emitter_prefab.h"

static void process_shot(Weapon *active_weapon) {
    if (weapon_has_ammo_in_mag(active_weapon)) {
        weapon_consume_ammo(active_weapon);

        active_weapon->render.muzzle_flash_timer = active_weapon->render.muzzle_flash_duration;
        active_weapon->render.recoil_timer = active_weapon->render.recoil_duration;

        if (active_weapon->type == WEAPON_PISTOL) {
            audio_bridge.pistol_shot = true;
        }

        if (active_weapon->type == WEAPON_RIFLE) {
            audio_bridge.rifle_shot = true;
        }
    }
}

static void process_hit(const Entity shooter_e, const Transform2D *player_transform, const Size2D *player_size,
                        const Weapon *active_weapon) {
    const float body_h = player_transform->scale.y * player_size->height;

    const WeaponRay ray = weapon_calculate_ray(player_transform->position, player_transform->rotation, body_h,
                                               &active_weapon->render
    );

    const float max_range = 1000.0f;
    const RaycastHit hit = raycast_cast(ray.front, ray.dir, max_range, shooter_e);

    if (hit.has_hit) {
        particle_emitter_prefab_create(hit.point, hit.normal, 0.2f);

        if (HAS_COMPONENT(range_target, hit.entity)) {
            RangeTarget *target = GET_COMPONENT(range_target, hit.entity);

            Collider2D *range_target_collider = GET_COMPONENT(collider, hit.entity);

            if (active_weapon->type == WEAPON_PISTOL && target->type == RANGE_TARGET_TYPE_LIGHT) {
                target->state = RANGE_TARGET_STATE_DESTROYED;
                range_target_collider->is_active = false;
            }

            if (active_weapon->type == WEAPON_RIFLE && target->type == RANGE_TARGET_TYPE_HEAVY) {
                target->state = RANGE_TARGET_STATE_DESTROYED;
                range_target_collider->is_active = false;
            }
        }
    }
}

static void init_reload(Player *p, Weapon *active_weapon) {
    if (weapon_has_ammo_reserve(active_weapon) && weapon_need_reload(active_weapon)) {
        p->action_state = PLAYER_ACTION_RELOADING;
        active_weapon->reload_timer = active_weapon->reload_duration;
    }
}

static void process_combat_actions(const float dt, const Entity p_e, Player *p, Inventory *inv,
                                   const Transform2D *player_transform, const Size2D *player_size) {
    Weapon *active_weapon = inventory_get_active_weapon(inv);

    if (p->process_shot) {
        if (p->stance_state == PLAYER_STANCE_AIMING && p->action_state == PLAYER_ACTION_DEFAULT) {
            process_shot(active_weapon);
            process_hit(p_e, player_transform, player_size, active_weapon);
        }

        p->process_shot = false;
    }

    if (p->process_reload) {
        if (p->action_state == PLAYER_ACTION_DEFAULT) {
            init_reload(p, active_weapon);
        }

        p->process_reload = false;
    }

    if (active_weapon->render.muzzle_flash_timer > 0.0f) {
        active_weapon->render.muzzle_flash_timer -= dt;
        if (active_weapon->render.muzzle_flash_timer < 0.0f) {
            active_weapon->render.muzzle_flash_timer = 0.0f;
        }
    }

    if (active_weapon->render.recoil_timer > 0.0f) {
        active_weapon->render.recoil_timer -= dt;
        if (active_weapon->render.recoil_timer < 0.0f) {
            active_weapon->render.recoil_timer = 0.0f;
        }
    }

    if (p->action_state == PLAYER_ACTION_RELOADING && active_weapon->reload_timer > 0.0f) {
        active_weapon->reload_timer -= dt;
        if (active_weapon->reload_timer <= 0.0f) {
            active_weapon->reload_timer = 0.0f;
            weapon_reload(active_weapon);
            p->action_state = PLAYER_ACTION_DEFAULT;
        }
    }
}

void combat_system_update(const float dt, const Entity p_e) {
    Player *p = GET_COMPONENT(player, p_e);
    Inventory *inv = GET_COMPONENT(inventory, p_e);
    const Transform2D *player_transform = GET_COMPONENT(transform, p_e);
    const Size2D *player_size = GET_COMPONENT(size, p_e);

    process_combat_actions(dt, p_e, p, inv, player_transform, player_size);
}
