#pragma once

#include "../ecs/ecs.h"

typedef enum {
    WEAPON_PISTOL,
    WEAPON_RIFLE,
    WEAPON_COUNT,
} WeaponType;

typedef struct {
    float width_ratio;
    float height_ratio;
    float corner_radius;
    float x_position_offset;
    float y_position_offset;
    float muzzle_flash_timer;
    float muzzle_flash_length;
    float muzzle_flash_width;
    float muzzle_flash_duration;
    float recoil_kick_distance;
    float recoil_duration;
    float recoil_timer;
} WeaponRender;

typedef struct {
    WeaponType type;
    float fire_rate;
    int ammo_in_mag;
    int mag_size;
    int reserve_ammo;
    int max_reserve_ammo;
    WeaponRender render;
    float reload_timer;
    float reload_duration;
} Weapon;

typedef struct {
    Weapon weapons[WEAPON_COUNT];
    int active_index;
} Inventory;

COMPONENT_DEFINE(Inventory, inventory);

static inline Weapon *inventory_get_active_weapon(Inventory *inv) {
    return &inv->weapons[inv->active_index];
}

static inline bool weapon_has_ammo_in_mag(const Weapon *w) {
    return w->ammo_in_mag > 0;
}

static inline bool weapon_has_ammo_reserve(const Weapon *w) {
    return w->reserve_ammo > 0;
}

static inline void weapon_consume_ammo(Weapon *w) {
    w->ammo_in_mag--;
}

static inline bool weapon_need_reload(const Weapon *w) {
    return w->ammo_in_mag < w->mag_size;
}

static inline void weapon_reload(Weapon *w) {
    const int needed = w->mag_size - w->ammo_in_mag;
    const int transfer = w->reserve_ammo < needed ? w->reserve_ammo : needed;

    w->ammo_in_mag += transfer;
    w->reserve_ammo -= transfer;
}

static inline void weapon_reset_reload_timer(Weapon *w) {
    w->reload_timer = 0.0f;
}

static inline void weapon_reset(Weapon *w) {
    w->render.muzzle_flash_timer = 0.0f;
    w->render.recoil_timer = 0.0f;
    weapon_reset_reload_timer(w);
}

static inline void weapon_add_reserve_ammo(Weapon *w, const int count) {
    w->reserve_ammo += count;

    if (w->reserve_ammo > w->max_reserve_ammo) {
        w->reserve_ammo = w->max_reserve_ammo;
    }
}

static inline Weapon *get_weapon_by_type(Inventory *inv, WeaponType type) {
    for (int i = 0; i < WEAPON_COUNT; ++i) {
        if (inv->weapons[i].type == type) {
            return &inv->weapons[i];
        }
    }

    return NULL;
}
