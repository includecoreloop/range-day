#pragma once

#include "../ecs/ecs.h"
#include "raylib.h"

extern Entity player_entity;
extern Camera2D game_camera;

void scene_init(void);

void scene_update(void);
