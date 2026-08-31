#include "raylib.h"

#include "../core/config.h"

#include "../systems/timer_system.h"
#include "../systems/raylib/render_system.h"
#include "../systems/movement_system.h"
#include "../systems/raylib/input_system.h"
#include "../systems/input_system.h"
#include "../systems/raylib/camera_system.h"
#include "../systems/rotation_system.h"
#include "../systems/collision_system.h"
#include "../systems/player_system.h"
#include "../systems/combat_system.h"
#include "../systems/range_target_system.h"
#include "../systems/raylib/audio_system.h"
#include "../systems/particle_emitter_system.h"
#include "demo.h"
#include "scene.h"

static void tick(const double dt) {
    input_system_update();
    timer_system_update((float) dt);
    movement_system_update(dt);
    player_system_update(player_entity);
    combat_system_update((float) dt, player_entity);
    leg_animation_system_update(dt);
    collision_system_update();
    rotation_system_update();
    range_target_system_update((float) dt);
    particle_emitter_system_update((float) dt);
    scene_update();
    camera_system_update();
}

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, WINDOW_TITLE);
    SetTargetFPS(TARGET_FPS);

    setup_demo_timer(2);
    scene_init();

    audio_system_init();

    double previous = GetTime();
    double accumulator = 0.0;

    while (!WindowShouldClose()) {
        const double current = GetTime();
        double frameTime = current - previous;

        if (frameTime > 0.25)
            frameTime = 0.25;

        previous = current;
        accumulator += frameTime;

        input_system_poll();

        while (accumulator >= FIXED_DT) {
            tick(FIXED_DT);
            accumulator -= FIXED_DT;
        }

        audio_system_update();
        render_system_update();
    }

    audio_system_destroy();

    CloseWindow();

    return 0;
}
