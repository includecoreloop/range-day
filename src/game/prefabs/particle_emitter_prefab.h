#pragma once

#include "../../ecs/ecs.h"
#include "../../core/math.h"

Entity particle_emitter_prefab_create(Vec2 position, Vec2 normal, float duration);

void particle_emitter_prefab_destroy(Entity emitter_entity);
