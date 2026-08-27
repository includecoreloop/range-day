#include "player_system.h"
#include "../game/profiles/player_collision_profile.h"
#include "../components/player.h"
#include "../components/collider.h"
#include "../components/inventory.h"

static void update_player_collision_profile(const Player *p, Collider2D *col, Inventory *inv) {
    bool has_weapon = false;
    WeaponType weapon_type = (WeaponType) 0;

    const Weapon *active_weapon = inventory_get_active_weapon(inv);

    if (active_weapon) {
        has_weapon = true;
        weapon_type = active_weapon->type;
    }

    player_collision_profile_apply(col, p->stance_state, weapon_type, has_weapon);
}

void player_system_update(const Entity p_e) {
    const Player *p = GET_COMPONENT(player, p_e);
    Collider2D *col = GET_COMPONENT(collider, p_e);
    Inventory *inv = GET_COMPONENT(inventory, p_e);

    update_player_collision_profile(p, col, inv);
}
