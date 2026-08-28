#pragma once

#include "../../ecs/ecs.h"
#include "../../core/math.h"
#include "../../components/size.h"

Entity obstacle_prefab_create(Vec2 position, Size2D obstacle_size);

void obstacle_prefab_destroy(Entity obstacle);
