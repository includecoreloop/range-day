#include "input_system.h"
#include "../core/input_bridge.h"
#include "../game/scene.h"
#include "../components/velocity.h"
#include "../components/player.h"
#include "../components/orientation_intent.h"
#include "../components/inventory.h"
#include <math.h>

static void player_cancel_reload(Player *p, Inventory *inv) {
    p->action_state = PLAYER_ACTION_DEFAULT;
    Weapon *active_weapon = inventory_get_active_weapon(inv);
    if (active_weapon) {
        weapon_reset(active_weapon);
    }
}

void input_system_update(void) {
    if (input_bridge.action_level_restart) {
        scene_destroy();
        scene_init();
        input_bridge_consume_action(&input_bridge.action_level_restart);
        return;
    }

    Player *p = GET_COMPONENT(player, player_entity);
    Velocity *v = GET_COMPONENT(velocity, player_entity);
    OrientationIntent *oi = GET_COMPONENT(orientation_intent, player_entity);
    Inventory *inv = GET_COMPONENT(inventory, player_entity);

    v->direction.x = 0.0f;
    v->direction.y = 0.0f;

    if (input_bridge.move_up) v->direction.y -= 1.0f;
    if (input_bridge.move_down) v->direction.y += 1.0f;
    if (input_bridge.move_left) v->direction.x -= 1.0f;
    if (input_bridge.move_right) v->direction.x += 1.0f;

    const float mag = sqrtf(v->direction.x * v->direction.x + v->direction.y * v->direction.y);
    const bool is_moving = mag > 0.0f;

    if (is_moving) {
        v->direction.x /= mag;
        v->direction.y /= mag;
    }

    p->stance_state = input_bridge.action_aim ? PLAYER_STANCE_AIMING : PLAYER_STANCE_NORMAL;

    if (!is_moving) {
        p->locomotion_state = PLAYER_LOCOMOTION_IDLE;
    } else if (input_bridge.action_run) {
        p->locomotion_state = PLAYER_LOCOMOTION_RUNNING;
    } else {
        p->locomotion_state = PLAYER_LOCOMOTION_WALKING;
    }

    if (p->stance_state == PLAYER_STANCE_AIMING) {
        v->speed = p->cqb_aim_speed;
        oi->look_at_pos = input_bridge.mouse_position;
        oi->look_at_moving_direction = false;

        p->process_shot = input_bridge.action_shoot;
        p->process_reload = input_bridge.action_reload;
    } else {
        if (p->action_state == PLAYER_ACTION_RELOADING) {
            player_cancel_reload(p, inv);
        }

        if (p->locomotion_state == PLAYER_LOCOMOTION_WALKING) {
            v->speed = p->walk_speed;
        } else if (p->locomotion_state == PLAYER_LOCOMOTION_RUNNING) {
            v->speed = p->run_speed;
        } else {
            v->speed = 0.0f;
        }

        oi->look_at_moving_direction = true;
    }

    input_bridge_consume_action(&input_bridge.action_shoot);
    input_bridge_consume_action(&input_bridge.action_reload);

    if (input_bridge.action_select_weapon_1) {
        if (inv->active_index != WEAPON_PISTOL) {
            player_cancel_reload(p, inv);
            inv->active_index = WEAPON_PISTOL;
        }
        input_bridge_consume_action(&input_bridge.action_select_weapon_1);
    } else if (input_bridge.action_select_weapon_2) {
        if (inv->active_index != WEAPON_RIFLE) {
            player_cancel_reload(p, inv);
            inv->active_index = WEAPON_RIFLE;
        }
        input_bridge_consume_action(&input_bridge.action_select_weapon_2);
    }
}
