#include "render_system.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <math.h>

#include "../../core/config.h"

#include "../../components/transform.h"
#include "../../components/render.h"
#include "../../components/humanoid_render.h"
#include "../../components/inventory.h"
#include "../../components/player.h"
#include "../../components/size.h"
#include "../../components/collider.h"
#include "../../components/range_target.h"
#include "../../components/particle_emitter.h"
#include "../../game/scene.h"

#include "../../physics/raycast.h"
#include "../../physics/weapon_ray.h"

static const float DEFAULT_OUTLINE_THICKNESS = 3.0f;

static const float DEFAULT_LASER_MAX_DISTANCE = 1000.0f;
static const float DEFAULT_LASER_LINE_THICKNESS = 1.5f;
static const float DEFAULT_LASER_HIT_RADIUS = 3.5f;

static const float DEFAULT_CORNER_RADIUS_RATIO = 0.10f;

static const int UI_FONT_SIZE = 40;
static const int UI_TUTORIAL_FONT_SIZE = 30;
static const int UI_MARGIN_X = 20;
static const int UI_LINE_HEIGHT = 40;

static inline Vector2
transform_local_to_world(const Vector2 local_pos, const Vector2 origin, const float rotation_deg) {
    return Vector2Add(origin, Vector2Rotate(local_pos, rotation_deg * DEG2RAD));
}

static void draw_vector_shape(const Vector2 center_pos, const float width, const float height,
                              const float rotation_deg, const int sides,
                              const Color fill_color, const Color outline_color) {
    const float radius = (width < height ? width : height) * 0.5f;

    rlPushMatrix();

    rlTranslatef(center_pos.x, center_pos.y, 0.0f);
    rlRotatef(rotation_deg, 0.0f, 0.0f, 1.0f);

    DrawPoly((Vector2){.x = 0, .y = 0}, sides, radius, 0.0f, fill_color);

    if (DEFAULT_OUTLINE_THICKNESS > 0.0f) {
        DrawPolyLinesEx((Vector2){.x = 0, .y = 0}, sides, radius, 0.0f, DEFAULT_OUTLINE_THICKNESS, outline_color);
    }

    rlPopMatrix();
}

static void draw_rounded_vector_rectangle(const Vector2 center_pos, const float width, const float height,
                                          const float rotation_deg, float corner_radius,
                                          const Color fill_color, const Color outline_color) {
    const float min_dim = width < height ? width : height;

    if (corner_radius > min_dim * 0.5f) {
        corner_radius = min_dim * 0.5f;
    }

    const float roundness = min_dim > 0.0f ? corner_radius * 2.0f / min_dim : 0.0f;

    const Rectangle rec = {.x = -width * 0.5f, .y = -height * 0.5f, .width = width, .height = height};

    rlPushMatrix();

    rlTranslatef(center_pos.x, center_pos.y, 0.0f);
    rlRotatef(rotation_deg, 0.0f, 0.0f, 1.0f);

    DrawRectangleRounded(rec, roundness, 8, fill_color);

    if (DEFAULT_OUTLINE_THICKNESS > 0.0f) {
        DrawRectangleRoundedLinesEx(rec, roundness, 8, DEFAULT_OUTLINE_THICKNESS, outline_color);
    }

    rlPopMatrix();
}

static void draw_humanoid_normal_stance(const Vector2 pos, const Transform2D *t, const HumanoidRender *hr,
                                        const float body_w, const float body_h, const Color color) {
    const float foot_w = body_w * hr->foot_width_ratio;
    const float foot_h = body_h * hr->foot_height_ratio;
    const float foot_spread = body_w * hr->foot_lateral_offset;
    const float foot_radius = foot_w * hr->foot_corner_radius;

    const float stride_dist = body_h * hr->current_stride_offset;
    const float left_y_offset = sinf(hr->leg_phase) * stride_dist;

    const Vector2 left_foot_local = {.x = -foot_spread, .y = left_y_offset};
    const Vector2 right_foot_local = {.x = foot_spread, .y = -left_y_offset};

    const Vector2 left_foot_pos = transform_local_to_world(left_foot_local, pos, t->rotation);
    const Vector2 right_foot_pos = transform_local_to_world(right_foot_local, pos, t->rotation);

    draw_rounded_vector_rectangle(left_foot_pos, foot_w, foot_h, t->rotation, foot_radius, color, RAYWHITE);
    draw_rounded_vector_rectangle(right_foot_pos, foot_w, foot_h, t->rotation, foot_radius, color, RAYWHITE);

    const float torso_corner_radius = body_w * hr->torso_corner_radius;
    draw_rounded_vector_rectangle(pos, body_w, body_h, t->rotation, torso_corner_radius, color, RAYWHITE);
}

static void draw_humanoid_pistol_stance(const Vector2 pos, const Transform2D *t, const HumanoidRender *hr,
                                        const float body_w, const float body_h, const float recoil_offset,
                                        const Color color) {
    const float torso_corner_radius = body_w * hr->torso_corner_radius;
    draw_rounded_vector_rectangle(pos, body_w, body_h, t->rotation, torso_corner_radius, color, RAYWHITE);

    const float arm_w = body_w * hr->pistol_arm_width_ratio;
    const float arm_h = body_h * hr->pistol_arm_height_ratio;
    const float arm_radius = arm_w * hr->arm_corner_radius;

    const float shoulder_x = body_w * hr->pistol_shoulder_x_ratio;
    const float shoulder_y = body_h * hr->pistol_shoulder_y_offset;

    const Vector2 l_shoulder_local = {.x = -shoulder_x, .y = shoulder_y};
    const Vector2 r_shoulder_local = {.x = shoulder_x, .y = shoulder_y};

    const Vector2 grip_local = {.x = 0.0f, .y = (body_h * hr->pistol_grip_y_offset) - recoil_offset};

    const Vector2 l_arm_local = Vector2Scale(Vector2Add(l_shoulder_local, grip_local), 0.5f);
    const Vector2 r_arm_local = Vector2Scale(Vector2Add(r_shoulder_local, grip_local), 0.5f);

    const Vector2 l_arm_pos = transform_local_to_world(l_arm_local, pos, t->rotation);
    const Vector2 r_arm_pos = transform_local_to_world(r_arm_local, pos, t->rotation);

    draw_rounded_vector_rectangle(l_arm_pos, arm_w, arm_h, t->rotation + hr->pistol_arm_aim_angle, arm_radius, color,
                                  RAYWHITE);
    draw_rounded_vector_rectangle(r_arm_pos, arm_w, arm_h, t->rotation - hr->pistol_arm_aim_angle, arm_radius, color,
                                  RAYWHITE);
}

static void draw_humanoid_rifle_stance(const Vector2 pos, const Transform2D *t, const HumanoidRender *hr,
                                       const float body_w, const float body_h, const float recoil_offset,
                                       const Color color) {
    const float torso_corner_radius = body_w * hr->torso_corner_radius;
    draw_rounded_vector_rectangle(pos, body_w, body_h, t->rotation + hr->rifle_aim_torso_angle, torso_corner_radius,
                                  color, RAYWHITE);

    const float arm_w = body_w * hr->rifle_arm_width_ratio;
    const float arm_h = body_h * hr->rifle_arm_height_ratio;
    const float arm_radius = arm_w * hr->arm_corner_radius;

    const float shoulder_x = body_w * hr->rifle_shoulder_x_ratio;
    const Vector2 l_shoulder_local = {.x = -shoulder_x, .y = body_h * hr->rifle_l_shoulder_y_offset};
    const Vector2 r_shoulder_local = {.x = shoulder_x, .y = body_h * hr->rifle_r_shoulder_y_offset};

    const Vector2 grip_local = {.x = 0.0f, .y = (body_h * hr->rifle_grip_y_offset) - recoil_offset};

    const Vector2 l_arm_local = {
        .x = (l_shoulder_local.x + grip_local.x) * hr->rifle_l_arm_midpoint_x_scale * 0.5f,
        .y = (l_shoulder_local.y + grip_local.y) * hr->rifle_l_arm_midpoint_y_scale * 0.5f
    };
    const Vector2 r_arm_local = Vector2Scale(Vector2Add(r_shoulder_local, grip_local), 0.5f);

    const Vector2 l_arm_pos = transform_local_to_world(l_arm_local, pos, t->rotation);
    const Vector2 r_arm_pos = transform_local_to_world(r_arm_local, pos, t->rotation);

    draw_rounded_vector_rectangle(l_arm_pos, arm_w, arm_h, t->rotation + hr->rifle_l_arm_aim_angle, arm_radius, color,
                                  RAYWHITE);
    draw_rounded_vector_rectangle(r_arm_pos, arm_w, arm_h, t->rotation - hr->rifle_r_arm_aim_angle, arm_radius, color,
                                  RAYWHITE);
}

static void draw_humanoid_weapon(const Vector2 pos, const Transform2D *t, const WeaponRender *wr, const float body_w,
                                 const float body_h, const float recoil_offset, const Color color) {
    const float gun_w = body_w * wr->width_ratio;
    const float gun_h = body_h * wr->height_ratio;
    const float gun_radius = gun_w * wr->corner_radius;

    const Vector2 gun_local = {
        .x = wr->x_position_offset,
        .y = body_h * wr->y_position_offset - recoil_offset
    };

    const Vector2 gun_pos = transform_local_to_world(gun_local, pos, t->rotation);

    draw_rounded_vector_rectangle(gun_pos, gun_w, gun_h, t->rotation, gun_radius, color, RAYWHITE);
}

static void draw_weapon_laser(const Entity entity_id, const WeaponRay *w_ray) {
    const Vector2 muzzle_pos = {.x = w_ray->front.x, .y = w_ray->front.y};

    const RaycastHit hit = raycast_cast(w_ray->front, w_ray->dir, DEFAULT_LASER_MAX_DISTANCE, entity_id);
    const Vector2 line_end = {.x = hit.point.x, .y = hit.point.y};

    DrawLineEx(muzzle_pos, line_end, DEFAULT_LASER_LINE_THICKNESS, RED);

    if (hit.has_hit) {
        DrawCircleV(line_end, DEFAULT_LASER_HIT_RADIUS, RED);
    }
}

static void draw_humanoid_head(const Vector2 pos, const Transform2D *t, const HumanoidRender *hr, const float body_w,
                               const float body_h, const Color color) {
    const float head_w = body_w * hr->head_width_ratio;
    const float head_h = body_h * hr->head_height_ratio;

    const Vector2 forward_local = {.x = 0.0f, .y = body_h * hr->head_forward_offset};
    const Vector2 head_pos = transform_local_to_world(forward_local, pos, t->rotation);

    draw_vector_shape(head_pos, head_w, head_h, t->rotation, 32, color, RAYWHITE);
}

static void draw_weapon_muzzle_flash(const Weapon *active_weapon, const WeaponRay *w_ray) {
    const WeaponRender *wr = &active_weapon->render;
    if (wr->muzzle_flash_timer <= 0.0f) return;

    const Vector2 muzzle_pos = {.x = w_ray->front.x, .y = w_ray->front.y};

    const float length_scale = (float) GetRandomValue(50, 150) * 0.01f;
    const float width_scale = (float) GetRandomValue(50, 120) * 0.01f;

    const float flash_length = wr->muzzle_flash_length * length_scale;
    const float flash_half_width = (wr->muzzle_flash_width * 0.5f) * width_scale;

    const Vector2 dir = {.x = w_ray->dir.x, .y = w_ray->dir.y};
    const Vector2 perpendicular = {.x = -dir.y, .y = dir.x};

    const Vector2 tip = Vector2Add(muzzle_pos, Vector2Scale(dir, flash_length));
    const Vector2 base_left = Vector2Add(muzzle_pos, Vector2Scale(perpendicular, flash_half_width));
    const Vector2 base_right = Vector2Subtract(muzzle_pos, Vector2Scale(perpendicular, flash_half_width));

    DrawTriangle(base_left, tip, base_right, RED);
}

static float weapon_render_get_recoil_offset(const WeaponRender *wr) {
    if (wr->recoil_timer <= 0.0f || wr->recoil_duration <= 0.0f)
        return 0.0f;

    const float t = wr->recoil_timer / wr->recoil_duration;

    return wr->recoil_kick_distance * (t * t);
}

static void draw_humanoid_entity(const Entity entity_id, const Vector2 pos, const Transform2D *t,
                                 const HumanoidRender *hr, const Size2D *s, const Color color) {
    const float body_w = t->scale.x * s->width;
    const float body_h = t->scale.y * s->height;

    bool is_aiming = false;

    if (HAS_COMPONENT(player, entity_id)) {
        const Player *p = GET_COMPONENT(player, entity_id);
        is_aiming = (p->stance_state == PLAYER_STANCE_AIMING);
    }

    if (!is_aiming) {
        draw_humanoid_normal_stance(pos, t, hr, body_w, body_h, color);
    } else if (HAS_COMPONENT(inventory, entity_id)) {
        Inventory *inv = GET_COMPONENT(inventory, entity_id);
        const Weapon *active_weapon = inventory_get_active_weapon(inv);

        if (active_weapon) {
            const WeaponRender *wr = &active_weapon->render;
            const float recoil_offset = weapon_render_get_recoil_offset(wr);

            switch (active_weapon->type) {
                case WEAPON_PISTOL:
                    draw_humanoid_pistol_stance(pos, t, hr, body_w, body_h, recoil_offset, color);
                    break;
                case WEAPON_RIFLE:
                    draw_humanoid_rifle_stance(pos, t, hr, body_w, body_h, recoil_offset, color);
                    break;
            }

            draw_humanoid_weapon(pos, t, wr, body_w, body_h, recoil_offset, color);

            const Vec2 world_pos = {.x = pos.x, .y = pos.y};

            WeaponRender wr_recoiled = *wr;
            wr_recoiled.y_position_offset -= recoil_offset / body_h;

            const WeaponRay w_ray = weapon_calculate_ray(world_pos, t->rotation, body_h, &wr_recoiled);

            draw_weapon_laser(entity_id, &w_ray);
            draw_weapon_muzzle_flash(active_weapon, &w_ray);
        }
    }

    draw_humanoid_head(pos, t, hr, body_w, body_h, color);
}

static void render_debug_colliders(void) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(transform, i) || !HAS_COMPONENT(collider, i)) continue;

        const Transform2D *t = GET_COMPONENT(transform, i);
        const Collider2D *c = GET_COMPONENT(collider, i);

        if (c->type == COLLIDER_CIRCLE) {
            for (int j = 0; j < c->circle_count; j++) {
                const CircleShape *circle = &c->circles[j];
                const Vector2 world_center =
                        transform_local_to_world(
                            (Vector2){.x = circle->offset.x, .y = circle->offset.y},
                            (Vector2){.x = t->position.x, .y = t->position.y},
                            t->rotation
                        );
                const float scaled_radius = circle->radius * t->scale.x;

                DrawCircleLinesV(world_center, scaled_radius, GREEN);

                const Vector2 forward_local = {.x = 0.0f, .y = -scaled_radius};
                const Vector2 forward_tip =
                        Vector2Add(world_center, Vector2Rotate(forward_local, t->rotation * DEG2RAD));

                DrawLineV(world_center, forward_tip, GREEN);
                DrawCircleV(world_center, 2.0f, GREEN);
            }
        } else if (c->type == COLLIDER_BOX) {
            const Vector2 world_center =
                    transform_local_to_world(
                        (Vector2){.x = c->offset.x, .y = c->offset.y},
                        (Vector2){.x = t->position.x, .y = t->position.y},
                        t->rotation
                    );

            const float half_w = c->half_extents.x * t->scale.x;
            const float half_h = c->half_extents.y * t->scale.y;

            const Vector2 corners[4] = {
                {.x = -half_w, .y = -half_h},
                {.x = half_w, .y = -half_h},
                {.x = half_w, .y = half_h},
                {.x = -half_w, .y = half_h}
            };

            Vector2 world_corners[4];

            for (int k = 0; k < 4; k++) {
                world_corners[k] = transform_local_to_world(corners[k], world_center, t->rotation);
            }

            for (int k = 0; k < 4; k++) {
                const Vector2 p1 = world_corners[k];
                const Vector2 p2 = world_corners[(k + 1) % 4];
                DrawLineEx(p1, p2, 1.5f, LIME);
            }
        }
    }
}

static void draw_range_target(const RangeTarget *rt, const Vector2 entity_pos, const Transform2D *t, const Size2D *s,
                              const Color color) {
    if (rt->state == RANGE_TARGET_STATE_VISIBLE) {
        const float width = s->width * t->scale.x;
        const float height = s->height * t->scale.y;
        const float corner_radius = (width < height ? width : height) * DEFAULT_CORNER_RADIUS_RATIO;

        const Color line_color = rt->type == RANGE_TARGET_TYPE_HEAVY ? SKYBLUE : MAGENTA;

        draw_rounded_vector_rectangle(entity_pos, width, height, t->rotation, corner_radius, color, line_color);
    }
}

static void render_ui() {
    const Player *p = GET_COMPONENT(player, player_entity);
    Inventory *inv = GET_COMPONENT(inventory, player_entity);

    const Weapon *active_weapon = inventory_get_active_weapon(inv);

    static const char *WEAPON_NAMES[] = {
        [WEAPON_PISTOL] = "PISTOL",
        [WEAPON_RIFLE] = "RIFLE",
    };

    DrawText(TextFormat("EQUIPPED : %s", WEAPON_NAMES[active_weapon->type]),
             UI_MARGIN_X, SCREEN_HEIGHT - UI_LINE_HEIGHT * 3, UI_FONT_SIZE, LIME);

    if (p->action_state == PLAYER_ACTION_RELOADING) {
        DrawText("RELOADING . . .",
                 UI_MARGIN_X, SCREEN_HEIGHT - UI_LINE_HEIGHT * 2, UI_FONT_SIZE, RED);
    } else {
        const Color mag_color = active_weapon->ammo_in_mag == 0 ? RED : LIME;
        DrawText(TextFormat("AMMO : %d / %d", active_weapon->ammo_in_mag, active_weapon->mag_size),
                 UI_MARGIN_X, SCREEN_HEIGHT - UI_LINE_HEIGHT * 2, UI_FONT_SIZE, mag_color);
    }

    const Color reserve_color = active_weapon->reserve_ammo == 0 ? RED : LIME;
    DrawText(TextFormat("RESERVE : %d / %d", active_weapon->reserve_ammo, active_weapon->max_reserve_ammo),
             UI_MARGIN_X, SCREEN_HEIGHT - UI_LINE_HEIGHT, UI_FONT_SIZE, reserve_color);

    static const char *STANDARD_CONTROLS[] = {
        "WASD : MOVE",
        "L SHIFT : SPRINT",
        "RIGHT MOUSE : AIM",
        "LEFT MOUSE : FIRE",
        "R : RELOAD (HOLD AIM)",
    };

    const int standard_count = sizeof(STANDARD_CONTROLS) / sizeof(STANDARD_CONTROLS[0]);

    for (int i = 0; i < standard_count; i++) {
        const int text_width = MeasureText(STANDARD_CONTROLS[i], UI_TUTORIAL_FONT_SIZE);
        const int x_pos = SCREEN_WIDTH - UI_MARGIN_X - text_width;
        const int y_pos = UI_MARGIN_X + i * UI_LINE_HEIGHT;

        DrawText(STANDARD_CONTROLS[i], x_pos, y_pos, UI_TUTORIAL_FONT_SIZE, LIGHTGRAY);
    }

    const char *pistol_str = "Q : PISTOL (LIGHT TARGETS)";
    int text_width = MeasureText(pistol_str, UI_TUTORIAL_FONT_SIZE);
    int x_pos = SCREEN_WIDTH - UI_MARGIN_X - text_width;
    int y_pos = UI_MARGIN_X + standard_count * UI_LINE_HEIGHT;

    DrawText(pistol_str, x_pos, y_pos, UI_TUTORIAL_FONT_SIZE, MAGENTA);

    const char *rifle_str = "E : RIFLE (HEAVY TARGETS)";
    text_width = MeasureText(rifle_str, UI_TUTORIAL_FONT_SIZE);
    x_pos = SCREEN_WIDTH - UI_MARGIN_X - text_width;
    y_pos = UI_MARGIN_X + (standard_count + 1) * UI_LINE_HEIGHT;

    DrawText(rifle_str, x_pos, y_pos, UI_TUTORIAL_FONT_SIZE, SKYBLUE);

    const char *restart_str = "P : RESTART LEVEL";
    text_width = MeasureText(restart_str, UI_TUTORIAL_FONT_SIZE);
    x_pos = SCREEN_WIDTH - UI_MARGIN_X - text_width;
    y_pos = UI_MARGIN_X + (standard_count + 2) * UI_LINE_HEIGHT;

    DrawText(restart_str, x_pos, y_pos, UI_TUTORIAL_FONT_SIZE, LIGHTGRAY);
}

static void draw_particle_emitter(const ParticleEmitter *emitter) {
    for (int i = 0; i < emitter->count; i++) {
        const Particle *p = &emitter->particles[i];

        if (p->lifetime >= p->max_lifetime) continue;

        const float alpha_factor = 1.0f - p->lifetime / p->max_lifetime;

        Color particle_color = GetColor(p->rgba);
        particle_color.a = (unsigned char) (particle_color.a * alpha_factor);
        DrawCircleV((Vector2){.x = p->position.x, .y = p->position.y}, p->size, particle_color);
    }
}

void render_system_update(void) {
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode2D(game_camera);

    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (HAS_COMPONENT(transform, i) && HAS_COMPONENT(render, i) && HAS_COMPONENT(size, i)) {
            const Transform2D *t = GET_COMPONENT(transform, i);
            Render *r = GET_COMPONENT(render, i);
            const Size2D *s = GET_COMPONENT(size, i);

            const Color color = RENDER_COLOR ? GetColor(r->rgba) : BLACK;
            const Vector2 entity_pos = {.x = t->position.x, .y = t->position.y};

            if (HAS_COMPONENT(humanoid_render, i)) {
                const HumanoidRender *hr = GET_COMPONENT(humanoid_render, i);
                draw_humanoid_entity(i, entity_pos, t, hr, s, color);
            } else if (HAS_COMPONENT(range_target, i)) {
                const RangeTarget *rt = GET_COMPONENT(range_target, i);
                draw_range_target(rt, entity_pos, t, s, color);
            } else {
                const float width = s->width * t->scale.x;
                const float height = s->height * t->scale.y;
                const float corner_radius = (width < height ? width : height) * DEFAULT_CORNER_RADIUS_RATIO;

                draw_rounded_vector_rectangle(entity_pos, width, height, t->rotation, corner_radius, color, RAYWHITE);
            }
        }

        if (HAS_COMPONENT(particle_emitter, i)) {
            const ParticleEmitter *emitter = GET_COMPONENT(particle_emitter, i);
            draw_particle_emitter(emitter);
        }
    }

    if (RENDER_COLLIDERS) {
        render_debug_colliders();
    }

    EndMode2D();

    if (RENDER_FPS_COUNTER) {
        DrawFPS(10, 10);
    }

    render_ui();

    EndDrawing();
}
