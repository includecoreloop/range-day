#pragma once

#include "../ecs/ecs.h"
#include "inventory.h"

typedef struct {
    WeaponType weapon_type;
    int ammo_count;
} PickupAmmo;

COMPONENT_DEFINE(PickupAmmo, pickup_ammo);