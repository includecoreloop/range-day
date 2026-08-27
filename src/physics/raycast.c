#include "raycast.h"
#include "../components/transform.h"
#include "../components/collider.h"
#include <math.h>

static Vec2 rotate_vec2(const Vec2 v, const float rad) {
    const float cos_a = cosf(rad);
    const float sin_a = sinf(rad);

    return (Vec2){
        .x = v.x * cos_a - v.y * sin_a,
        .y = v.x * sin_a + v.y * cos_a
    };
}

static bool intersect_ray_circle(const Vec2 origin, const Vec2 dir, const Vec2 center, const float radius,
                                 const float max_dist, float *out_t,
                                 Vec2 *out_normal) {
    const Vec2 f = {.x = origin.x - center.x, .y = origin.y - center.y};

    const float b = 2.0f * (f.x * dir.x + f.y * dir.y);
    const float c = (f.x * f.x + f.y * f.y) - (radius * radius);
    const float discriminant = (b * b) - (4.0f * c);

    if (discriminant < 0.0f) return false;

    const float sqrt_d = sqrtf(discriminant);
    const float t1 = (-b - sqrt_d) * 0.5f;
    const float t2 = (-b + sqrt_d) * 0.5f;

    float hit_t = -1.0f;

    if (t1 >= 0.0f && t1 <= max_dist) {
        hit_t = t1;
    } else if (t2 >= 0.0f && t2 <= max_dist) {
        hit_t = t2;
    }

    if (hit_t < 0.0f) return false;

    *out_t = hit_t;

    if (out_normal) {
        const Vec2 hit_pt = {.x = origin.x + dir.x * hit_t, .y = origin.y + dir.y * hit_t};
        const Vec2 norm = {.x = hit_pt.x - center.x, .y = hit_pt.y - center.y};
        const float len = sqrtf(norm.x * norm.x + norm.y * norm.y);

        if (len > 0.0001f) {
            out_normal->x = norm.x / len;
            out_normal->y = norm.y / len;
        } else {
            *out_normal = (Vec2){.x = 0.0f, .y = -1.0f};
        }
    }

    return true;
}

static bool intersect_ray_box(const Vec2 origin, const Vec2 dir, const Vec2 box_center, const Vec2 half_extents,
                              const float rotation_deg,
                              const float max_dist, float *out_t, Vec2 *out_normal) {
    const float rad = -rotation_deg * DEG2RAD;
    const Vec2 local_origin = rotate_vec2((Vec2){.x = origin.x - box_center.x, .y = origin.y - box_center.y}, rad);
    const Vec2 local_dir = rotate_vec2(dir, rad);

    const float half_w = half_extents.x;
    const float half_h = half_extents.y;

    float t_min = 0.0f;
    float t_max = max_dist;

    Vec2 local_norm = {.x = 0.0f, .y = 0.0f};

    if (fabsf(local_dir.x) < 1e-6f) {
        if (local_origin.x < -half_w || local_origin.x > half_w) return false;
    } else {
        const float inv_d = 1.0f / local_dir.x;
        float t1 = (-half_w - local_origin.x) * inv_d;
        float t2 = (half_w - local_origin.x) * inv_d;

        Vec2 n1 = {.x = -1.0f, .y = 0.0f};
        Vec2 n2 = {.x = 1.0f, .y = 0.0f};

        if (t1 > t2) {
            const float tmp = t1;
            t1 = t2;
            t2 = tmp;
            const Vec2 n_tmp = n1;
            n1 = n2;
            n2 = n_tmp;
        }

        if (t1 > t_min) {
            t_min = t1;
            local_norm = n1;
        }

        if (t2 < t_max) t_max = t2;

        if (t_min > t_max) return false;
    }

    if (fabsf(local_dir.y) < 1e-6f) {
        if (local_origin.y < -half_h || local_origin.y > half_h) return false;
    } else {
        const float inv_d = 1.0f / local_dir.y;
        float t1 = (-half_h - local_origin.y) * inv_d;
        float t2 = (half_h - local_origin.y) * inv_d;

        Vec2 n1 = {.x = 0.0f, .y = -1.0f};
        Vec2 n2 = {.x = 0.0f, .y = 1.0f};

        if (t1 > t2) {
            const float tmp = t1;
            t1 = t2;
            t2 = tmp;
            const Vec2 n_tmp = n1;
            n1 = n2;
            n2 = n_tmp;
        }

        if (t1 > t_min) {
            t_min = t1;
            local_norm = n1;
        }

        if (t2 < t_max) t_max = t2;

        if (t_min > t_max) return false;
    }

    if (t_min < 0.0f || t_min > max_dist) return false;

    *out_t = t_min;

    if (out_normal) {
        *out_normal = rotate_vec2(local_norm, rotation_deg * DEG2RAD);
    }

    return true;
}

RaycastHit raycast_cast(const Vec2 origin, const Vec2 dir, const float max_dist, const Entity ignore_entity) {
    RaycastHit hit = {
        .has_hit = false,
        .entity = 0,
        .point = {.x = origin.x + dir.x * max_dist, .y = origin.y + dir.y * max_dist},
        .normal = {.x = 0.0f, .y = 0.0f},
        .distance = max_dist
    };

    float closest_t = max_dist;

    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (i == ignore_entity) continue;

        if (HAS_COMPONENT(transform, i) && HAS_COMPONENT(collider, i)) {
            const Transform2D *t = GET_COMPONENT(transform, i);
            const Collider2D *c = GET_COMPONENT(collider, i);

            if (!c->is_active) continue;
            if (c->is_trigger) continue;

            const float rad = t->rotation * DEG2RAD;

            if (c->type == COLLIDER_CIRCLE) {
                for (int j = 0; j < c->circle_count; j++) {
                    const CircleShape *circle = &c->circles[j];

                    const Vec2 rotated_offset = rotate_vec2(circle->offset, rad);
                    const Vec2 circle_center = {
                        .x = t->position.x + rotated_offset.x,
                        .y = t->position.y + rotated_offset.y
                    };

                    const float circle_radius = circle->radius * t->scale.x;

                    float t_hit = 0.0f;
                    Vec2 norm_hit = {.x = 0.0f, .y = 0.0f};

                    if (intersect_ray_circle(origin, dir, circle_center, circle_radius, closest_t, &t_hit, &norm_hit)) {
                        if (t_hit >= 0.0f && t_hit < closest_t) {
                            closest_t = t_hit;
                            hit.has_hit = true;
                            hit.entity = i;
                            hit.distance = t_hit;
                            hit.point = (Vec2){.x = origin.x + dir.x * t_hit, .y = origin.y + dir.y * t_hit};
                            hit.normal = norm_hit;
                        }
                    }
                }
            } else if (c->type == COLLIDER_BOX) {
                const Vec2 rotated_offset = rotate_vec2(c->offset, rad);
                const Vec2 box_center = {
                    .x = t->position.x + rotated_offset.x,
                    .y = t->position.y + rotated_offset.y
                };

                const Vec2 scaled_half_extents = {
                    .x = c->half_extents.x * t->scale.x,
                    .y = c->half_extents.y * t->scale.y
                };

                float t_hit = 0.0f;
                Vec2 norm_hit = {.x = 0.0f, .y = 0.0f};

                if (intersect_ray_box(origin, dir, box_center, scaled_half_extents, t->rotation, closest_t, &t_hit,
                                      &norm_hit)) {
                    if (t_hit >= 0.0f && t_hit < closest_t) {
                        closest_t = t_hit;
                        hit.has_hit = true;
                        hit.entity = i;
                        hit.distance = t_hit;
                        hit.point = (Vec2){.x = origin.x + dir.x * t_hit, .y = origin.y + dir.y * t_hit};
                        hit.normal = norm_hit;
                    }
                }
            }
        }
    }

    return hit;
}
