#pragma once

#include <stdbool.h>
#include "math.h"

typedef struct {
    bool move_up;
    bool move_down;
    bool move_left;
    bool move_right;

    bool action_pause;
    bool action_level_restart;

    Vec2 mouse_position;

    bool action_aim;
    bool action_shoot;
    bool action_reload;
    bool action_run;

    bool action_select_weapon_1;
    bool action_select_weapon_2;
} InputBridge;

extern InputBridge input_bridge;

static inline void input_bridge_consume_action(bool *action) {
    *action = false;
}
