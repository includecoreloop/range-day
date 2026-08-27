cmake_minimum_required(VERSION 4.0)
project(range-day C)

set(CMAKE_C_STANDARD 17)

add_executable(range-day src/game/main.c)

target_sources(range-day PRIVATE
        src/core/config.h
        src/core/input_bridge.h
        src/core/audio_bridge.h
        src/core/math.h
        src/ecs/ecs.c
        src/ecs/ecs.h
        src/physics/raycast.c
        src/physics/raycast.h
        src/physics/weapon_ray.c
        src/physics/weapon_ray.h
)

target_sources(range-day PRIVATE
        src/components/collider.c
        src/components/collider.h
        src/components/humanoid_render.c
        src/components/humanoid_render.h
        src/components/inventory.c
        src/components/inventory.h
        src/components/orientation_intent.c
        src/components/orientation_intent.h
        src/components/player.c
        src/components/player.h
        src/components/render.c
        src/components/render.h
        src/components/size.c
        src/components/size.h
        src/components/timer.c
        src/components/timer.h
        src/components/transform.c
        src/components/transform.h
        src/components/velocity.c
        src/components/velocity.h
        src/components/range_target.c
        src/components/range_target.h
)

target_sources(range-day PRIVATE
        src/systems/collision_system.c
        src/systems/collision_system.h
        src/systems/input_system.c
        src/systems/input_system.h
        src/systems/movement_system.c
        src/systems/movement_system.h
        src/systems/player_system.c
        src/systems/player_system.h
        src/systems/rotation_system.c
        src/systems/rotation_system.h
        src/systems/timer_system.c
        src/systems/timer_system.h
        src/systems/raylib/camera_system.c
        src/systems/raylib/camera_system.h
        src/systems/raylib/input_system.c
        src/systems/raylib/input_system.h
        src/systems/raylib/render_system.c
        src/systems/raylib/render_system.h
        src/systems/raylib/audio_system.c
        src/systems/raylib/audio_system.h
        src/systems/combat_system.c
        src/systems/combat_system.h
        src/systems/range_target_system.c
        src/systems/range_target_system.h
)

target_sources(range-day PRIVATE
        src/game/demo.c
        src/game/demo.h
        src/game/scene.c
        src/game/scene.h
        src/game/prefabs/player_prefab.c
        src/game/prefabs/player_prefab.h
        src/game/profiles/player_collision_profile.c
        src/game/profiles/player_collision_profile.h
        src/game/prefabs/range_target_prefab.c
        src/game/prefabs/range_target_prefab.h
        src/game/prefabs/obstacle_prefab.c
        src/game/prefabs/obstacle_prefab.h
)

if (MSVC)
    set(RAYLIB_SUBDIR "msvc")
    set(RAYLIB_LIB_NAME "raylib.lib")
    set(PLATFORM_LIBS winmm gdi32 user32 shell32)
elseif (MINGW)
    set(RAYLIB_SUBDIR "mingw")
    set(RAYLIB_LIB_NAME "libraylib.a")
    set(PLATFORM_LIBS winmm gdi32 user32 shell32)
elseif (APPLE)
    set(RAYLIB_SUBDIR "macos")
    set(RAYLIB_LIB_NAME "libraylib.a")
    set(PLATFORM_LIBS "-framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo")
else ()
    message(FATAL_ERROR "Unsupported platform or compiler!")
endif ()

target_include_directories(range-day PRIVATE "${CMAKE_SOURCE_DIR}/external/raylib/${RAYLIB_SUBDIR}/include")

target_link_libraries(range-day PRIVATE
        "${CMAKE_SOURCE_DIR}/external/raylib/${RAYLIB_SUBDIR}/lib/${RAYLIB_LIB_NAME}"
        ${PLATFORM_LIBS}
)