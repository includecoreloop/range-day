#include "scene.h"
#include "../core/config.h"
#include "../components/transform.h"
#include "../components/collider.h"
#include "../components/size.h"

#include "prefabs/player_prefab.h"
#include "prefabs/range_target_prefab.h"
#include "prefabs/obstacle_prefab.h"
#include "prefabs/pickup_ammo_prefab.h"

Entity player_entity = 0;
Camera2D game_camera = {0};

void scene_init(void) {
    game_camera.target = (Vector2){.x = SCREEN_CENTER_X, .y = SCREEN_CENTER_Y};
    game_camera.offset = (Vector2){.x = SCREEN_CENTER_X, .y = SCREEN_CENTER_Y};
    game_camera.rotation = 0.0f;
    game_camera.zoom = 1.0f;

    player_entity = player_prefab_create();

    range_target_prefab_create((Vec2){.x = 240.0f, .y = 780.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 1.0f, 4.0f);
    range_target_prefab_create((Vec2){.x = 1400.0f, .y = 680.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_HEAVY, 3.0f, 5.0f);
    range_target_prefab_create((Vec2){.x = 30.0f, .y = 660.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 5.0f, 3.5f);
    range_target_prefab_create((Vec2){.x = 1300.0f, .y = 580.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 7.0f, 3.0f);
    range_target_prefab_create((Vec2){.x = 200.0f, .y = 600.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_HEAVY, 9.0f, 4.5f);
    range_target_prefab_create((Vec2){.x = 900.0f, .y = 580.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 11.0f, 3.5f);
    range_target_prefab_create((Vec2){.x = 40.0f, .y = 500.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 13.0f, 4.0f);

    range_target_prefab_create((Vec2){.x = 1100.0f, .y = 540.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_HEAVY, 15.5f, 5.0f);
    range_target_prefab_create((Vec2){.x = 60.0f, .y = 460.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 17.5f, 3.0f);
    range_target_prefab_create((Vec2){.x = 1300.0f, .y = 440.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 19.5f, 3.5f);
    range_target_prefab_create((Vec2){.x = 60.0f, .y = 300.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_HEAVY, 21.5f, 4.0f);
    range_target_prefab_create((Vec2){.x = 1200.0f, .y = 380.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 22.5f, 3.5f);
    range_target_prefab_create((Vec2){.x = 1000.0f, .y = 340.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 25.0f, 4.0f);
    range_target_prefab_create((Vec2){.x = 280.0f, .y = 300.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 27.0f, 3.0f);
    range_target_prefab_create((Vec2){.x = 1300.0f, .y = 280.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_HEAVY, 29.0f, 4.5f);
    range_target_prefab_create((Vec2){.x = 900.0f, .y = 300.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 31.0f, 3.5f);
    range_target_prefab_create((Vec2){.x = 300.0f, .y = 260.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 33.0f, 4.0f);

    range_target_prefab_create((Vec2){.x = 1100.0f, .y = 340.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_HEAVY, 35.5f, 5.0f);
    range_target_prefab_create((Vec2){.x = 50.0f, .y = 220.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 37.5f, 3.0f);
    range_target_prefab_create((Vec2){.x = 200.0f, .y = 200.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 39.5f, 3.5f);
    range_target_prefab_create((Vec2){.x = 1400.0f, .y = 140.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_HEAVY, 41.5f, 4.0f);
    range_target_prefab_create((Vec2){.x = 1000.0f, .y = 150.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 43.5f, 3.0f);
    range_target_prefab_create((Vec2){.x = 1100.0f, .y = 120.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 45.5f, 3.5f);
    range_target_prefab_create((Vec2){.x = 200.0f, .y = 100.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_HEAVY, 47.5f, 4.5f);
    range_target_prefab_create((Vec2){.x = 1200.0f, .y = 80.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 49.5f, 3.0f);
    range_target_prefab_create((Vec2){.x = 150.0f, .y = 50.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 51.5f, 4.0f);
    range_target_prefab_create((Vec2){.x = 800.0f, .y = 50.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_HEAVY, 53.5f, 4.5f);
    range_target_prefab_create((Vec2){.x = 1000.0f, .y = 0.0f}, (Size2D){.width = 60.0f, .height = 60.0f},
                               RANGE_TARGET_TYPE_LIGHT, 56.0f, 5.0f);

    obstacle_prefab_create((Vec2){.x = 400.0f, .y = 200.0f}, (Size2D){.width = 30.0f, .height = 100.0f});
    obstacle_prefab_create((Vec2){.x = 300.0f, .y = 0.0f}, (Size2D){.width = 30.0f, .height = 100.0f});
    obstacle_prefab_create((Vec2){.x = 300.0f, .y = 400.0f}, (Size2D){.width = 200.0f, .height = 30.0f});
    obstacle_prefab_create((Vec2){.x = 350.0f, .y = 600.0f}, (Size2D){.width = 30.0f, .height = 150.0f});

    obstacle_prefab_create((Vec2){.x = 900.0f, .y = 100.0f}, (Size2D){.width = 30.0f, .height = 100.0f});
    obstacle_prefab_create((Vec2){.x = 1200.0f, .y = 200.0f}, (Size2D){.width = 300.0f, .height = 30.0f});
    obstacle_prefab_create((Vec2){.x = 800.0f, .y = 300.0f}, (Size2D){.width = 30.0f, .height = 100.0f});
    obstacle_prefab_create((Vec2){.x = 1000.0f, .y = 500.0f}, (Size2D){.width = 30.0f, .height = 100.0f});
    obstacle_prefab_create((Vec2){.x = 1000.0f, .y = 700.0f}, (Size2D){.width = 300.0f, .height = 30.0f});

    pickup_ammo_prefab_create((Vec2){.x = 450.0f, .y = 700.0f}, WEAPON_PISTOL, 12);
    pickup_ammo_prefab_create((Vec2){.x = 550.0f, .y = 400.0f}, WEAPON_RIFLE, 25);
    pickup_ammo_prefab_create((Vec2){.x = 650.0f, .y = 100.0f}, WEAPON_PISTOL, 12);
    pickup_ammo_prefab_create((Vec2){.x = 450.0f, .y = -100.0f}, WEAPON_RIFLE, 25);
}

void scene_update(void) {
}

void scene_destroy(void) {
    player_prefab_destroy(player_entity);

    for (int i = 0; i < MAX_ENTITIES; ++i) {
        if (HAS_COMPONENT(range_target, i)) {
            range_target_prefab_destroy(i);
        } else if (HAS_COMPONENT(transform, i)) {
            obstacle_prefab_destroy(i);
        }
    }

    player_entity = 0;
    game_camera = (Camera2D){0};
}
