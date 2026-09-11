#pragma once

#include "../../ecs/ecs.h"
#include "../../core/math.h"
#include "../../components/inventory.h"

Entity pickup_ammo_prefab_create(Vec2 position, WeaponType weapon_type, int ammo_count);

void pickup_ammo_prefab_destroy(Entity entity);