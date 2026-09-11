#include "pickup_system.h"

#include "../ecs/ecs.h"

#include "../components/inventory.h"
#include "../components/collider.h"
#include "../components/pickup_ammo.h"
#include "../game/prefabs/pickup_ammo_prefab.h"


void pickup_system_update(void) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(pickup_ammo, i) || !HAS_COMPONENT(collider, i)) continue;

        const Collider2D *col = GET_COMPONENT(collider, i);
        const PickupAmmo *pickup = GET_COMPONENT(pickup_ammo, i);

        for (int c = 0; c < col->trigger_contact_count; c++) {
            const Entity other_entity = col->triggered_entities[c];

            if (HAS_COMPONENT(inventory, other_entity)) {
                Inventory *inv = GET_COMPONENT(inventory, other_entity);

                Weapon *w = get_weapon_by_type(inv, pickup->weapon_type);
                weapon_add_reserve_ammo(w, pickup->ammo_count);

                pickup_ammo_prefab_destroy(i);

                break;
            }
        }
    }
}
