#pragma once

#include "../../ecs/ecs.h"
#include "../../core/math.h"
#include "../../components/size.h"
#include "../../components/range_target.h"

Entity range_target_prefab_create(Vec2 position, Size2D target_size, RangeTargetType target_type, float spawn_delay,
                                  float active_duration);
