#pragma once

#include "../ecs/ecs.h"

typedef enum {
    PLAYER_LOCOMOTION_IDLE,
    PLAYER_LOCOMOTION_WALKING,
    PLAYER_LOCOMOTION_RUNNING,
} PlayerLocomotionState;

typedef enum {
    PLAYER_STANCE_NORMAL,
    PLAYER_STANCE_AIMING,
} PlayerStanceState;

typedef enum {
    PLAYER_ACTION_DEFAULT,
    PLAYER_ACTION_RELOADING,
} PlayerActionState;

typedef struct {
    PlayerLocomotionState locomotion_state;
    PlayerStanceState stance_state;
    PlayerActionState action_state;

    float run_speed;
    float walk_speed;
    float cqb_aim_speed;

    bool process_shot;
    bool process_reload;
} Player;

COMPONENT_DEFINE(Player, player);
