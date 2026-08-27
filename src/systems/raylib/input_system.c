#include "raylib.h"
#include "../../core/input_bridge.h"
#include "../../game/scene.h"

InputBridge input_bridge = {0};

void input_system_poll(void) {
    input_bridge.move_up = IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    input_bridge.move_down = IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN);
    input_bridge.move_left = IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT);
    input_bridge.move_right = IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT);

    input_bridge.action_pause |= IsKeyPressed(KEY_ESCAPE);

    input_bridge.action_aim = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);
    input_bridge.action_shoot |= IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
    input_bridge.action_reload |= IsKeyPressed(KEY_R);
    input_bridge.action_run = IsKeyDown(KEY_LEFT_SHIFT);

    input_bridge.action_select_weapon_1 |= IsKeyPressed(KEY_Q);
    input_bridge.action_select_weapon_2 |= IsKeyPressed(KEY_E);

    const Vector2 screen_mouse = GetMousePosition();
    const Vector2 world_mouse = GetScreenToWorld2D(screen_mouse, game_camera);

    input_bridge.mouse_position.x = world_mouse.x;
    input_bridge.mouse_position.y = world_mouse.y;
}
