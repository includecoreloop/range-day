#include "collision_system.h"
#include "../components/transform.h"
#include "../components/collider.h"
#include "../core/math.h"
#include <math.h>

static Vec2 get_circle_world_center(const Transform2D *t, const CircleShape circle) {
    if (circle.offset.x == 0.0f && circle.offset.y == 0.0f) {
        return t->position;
    }

    const float rad = t->rotation * DEG2RAD;
    const float cos_a = cosf(rad);
    const float sin_a = sinf(rad);

    return (Vec2){
        .x = t->position.x + (circle.offset.x * cos_a - circle.offset.y * sin_a),
        .y = t->position.y + (circle.offset.x * sin_a + circle.offset.y * cos_a)
    };
}

static Vec2 get_box_world_center(const Transform2D *t, const Collider2D *c) {
    if (c->offset.x == 0.0f && c->offset.y == 0.0f) {
        return t->position;
    }

    const float rad = t->rotation * DEG2RAD;
    const float cos_a = cosf(rad);
    const float sin_a = sinf(rad);

    return (Vec2){
        .x = t->position.x + (c->offset.x * cos_a - c->offset.y * sin_a),
        .y = t->position.y + (c->offset.x * sin_a + c->offset.y * cos_a)
    };
}

static bool is_circle_vs_box_overlapping(const Transform2D *circle_t, const CircleShape circle,
                                         const Transform2D *box_t, const Collider2D *box_col) {
    const Vec2 circle_center = get_circle_world_center(circle_t, circle);
    const Vec2 box_center = get_box_world_center(box_t, box_col);

    const Vec2 box_min = {.x = box_center.x - box_col->half_extents.x, .y = box_center.y - box_col->half_extents.y};
    const Vec2 box_max = {.x = box_center.x + box_col->half_extents.x, .y = box_center.y + box_col->half_extents.y};

    const float closest_x = math_clamp(circle_center.x, box_min.x, box_max.x);
    const float closest_y = math_clamp(circle_center.y, box_min.y, box_max.y);

    const float diff_x = circle_center.x - closest_x;
    const float diff_y = circle_center.y - closest_y;

    const float distance_sq = diff_x * diff_x + diff_y * diff_y;

    return distance_sq < circle.radius * circle.radius;
}

static bool is_circle_vs_circle_overlapping(const Transform2D *t_a, const CircleShape circle_a,
                                            const Transform2D *t_b, const CircleShape circle_b) {
    const Vec2 center_a = get_circle_world_center(t_a, circle_a);
    const Vec2 center_b = get_circle_world_center(t_b, circle_b);

    const float diff_x = center_a.x - center_b.x;
    const float diff_y = center_a.y - center_b.y;

    const float sum_radii = circle_a.radius + circle_b.radius;
    const float distance_sq = diff_x * diff_x + diff_y * diff_y;

    return distance_sq < sum_radii * sum_radii;
}

static bool is_overlapping(const Transform2D *t_a, const Collider2D *c_a, const Transform2D *t_b,
                           const Collider2D *c_b) {
    if (c_a->type == COLLIDER_CIRCLE && c_b->type == COLLIDER_BOX) {
        for (int k = 0; k < c_a->circle_count; k++) {
            if (is_circle_vs_box_overlapping(t_a, c_a->circles[k], t_b, c_b))
                return true;
        }
    } else if (c_a->type == COLLIDER_BOX && c_b->type == COLLIDER_CIRCLE) {
        for (int k = 0; k < c_b->circle_count; k++) {
            if (is_circle_vs_box_overlapping(t_b, c_b->circles[k], t_a, c_a))
                return true;
        }
    } else if (c_a->type == COLLIDER_CIRCLE && c_b->type == COLLIDER_CIRCLE) {
        for (int ka = 0; ka < c_a->circle_count; ka++) {
            for (int kb = 0; kb < c_b->circle_count; kb++) {
                if (is_circle_vs_circle_overlapping(t_a, c_a->circles[ka], t_b, c_b->circles[kb]))
                    return true;
            }
        }
    }

    return false;
}

static void apply_position_displacement(Transform2D *t_a, Transform2D *t_b, const Vec2 normal, const float penetration,
                                        const bool b_is_static) {
    if (penetration <= 0.0f) return;

    if (b_is_static) {
        t_a->position.x += normal.x * penetration;
        t_a->position.y += normal.y * penetration;
    } else {
        const float push_half = penetration * 0.5f;
        t_a->position.x += normal.x * push_half;
        t_a->position.y += normal.y * push_half;

        t_b->position.x -= normal.x * push_half;
        t_b->position.y -= normal.y * push_half;
    }
}

static void resolve_deep_circle_in_box(Transform2D *circle_t, const float radius, const Vec2 circle_center,
                                       const Vec2 box_min, const Vec2 box_max, Transform2D *box_t,
                                       const bool box_is_static) {
    const float dist_left = circle_center.x - box_min.x;
    const float dist_right = box_max.x - circle_center.x;
    const float dist_top = circle_center.y - box_min.y;
    const float dist_bottom = box_max.y - circle_center.y;

    float min_dist = dist_left;
    Vec2 normal = (Vec2){.x = -1.0f, .y = 0.0f};

    if (dist_right < min_dist) {
        min_dist = dist_right;
        normal = (Vec2){.x = 1.0f, .y = 0.0f};
    }
    if (dist_top < min_dist) {
        min_dist = dist_top;
        normal = (Vec2){.x = 0.0f, .y = -1.0f};
    }
    if (dist_bottom < min_dist) {
        min_dist = dist_bottom;
        normal = (Vec2){.x = 0.0f, .y = 1.0f};
    }

    const float penetration = min_dist + radius;

    apply_position_displacement(circle_t, box_t, normal, penetration, box_is_static);
}

static void resolve_single_circle_vs_circle(Transform2D *t_a, const CircleShape circle_a,
                                            Transform2D *t_b, const CircleShape circle_b,
                                            const bool b_is_static) {
    const Vec2 center_a = get_circle_world_center(t_a, circle_a);
    const Vec2 center_b = get_circle_world_center(t_b, circle_b);

    const Vec2 diff = {.x = center_a.x - center_b.x, .y = center_a.y - center_b.y};
    const float distance = sqrtf((diff.x * diff.x) + (diff.y * diff.y));

    if (distance == 0.0f) return;

    const float sum_radii = circle_a.radius + circle_b.radius;
    if (distance >= sum_radii) return;

    const float penetration = sum_radii - distance;
    const Vec2 normal = {.x = diff.x / distance, .y = diff.y / distance};

    apply_position_displacement(t_a, t_b, normal, penetration, b_is_static);
}

static void resolve_single_circle_vs_box(Transform2D *circle_t, const CircleShape circle,
                                         Transform2D *box_t, const Collider2D *box_col) {
    const Vec2 circle_center = get_circle_world_center(circle_t, circle);
    const Vec2 box_center = get_box_world_center(box_t, box_col);

    const Vec2 box_min = {.x = box_center.x - box_col->half_extents.x, .y = box_center.y - box_col->half_extents.y};
    const Vec2 box_max = {.x = box_center.x + box_col->half_extents.x, .y = box_center.y + box_col->half_extents.y};

    const Vec2 closest = {
        .x = math_clamp(circle_center.x, box_min.x, box_max.x),
        .y = math_clamp(circle_center.y, box_min.y, box_max.y)
    };

    const Vec2 diff = {.x = circle_center.x - closest.x, .y = circle_center.y - closest.y};
    const float distance_sq = (diff.x * diff.x) + (diff.y * diff.y);

    if (distance_sq == 0.0f) {
        resolve_deep_circle_in_box(circle_t, circle.radius, circle_center, box_min, box_max, box_t, box_col->is_static);
        return;
    }

    const float distance = sqrtf(distance_sq);
    if (distance >= circle.radius) return;

    const float penetration = circle.radius - distance;
    const Vec2 normal = {.x = diff.x / distance, .y = diff.y / distance};

    apply_position_displacement(circle_t, box_t, normal, penetration, box_col->is_static);
}

static void reset_all_trigger_contacts(void) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(collider, i)) continue;
        Collider2D *c = GET_COMPONENT(collider, i);
        c->trigger_contact_count = 0;
    }
}

static bool can_collide(const Collider2D *c_a, const Collider2D *c_b) {
    if (!c_a->is_active || !c_b->is_active) return false;

    if (c_a->is_trigger && c_b->is_trigger) return false;

    if (c_a->is_static && c_b->is_static) return false;

    return true;
}

static bool resolve_trigger_overlap(const Entity entity_a, const Transform2D *t_a, Collider2D *c_a,
                                    const Entity entity_b, const Transform2D *t_b, Collider2D *c_b) {
    if (!c_a->is_trigger && !c_b->is_trigger) return false;

    if (is_overlapping(t_a, c_a, t_b, c_b)) {
        collider_add_trigger_contact(c_a, entity_b);
        collider_add_trigger_contact(c_b, entity_a);
    }

    return true;
}

static void resolve_physics_collision(Transform2D *t_a, const Collider2D *c_a,
                                      Transform2D *t_b, const Collider2D *c_b) {
    if (c_a->type == COLLIDER_CIRCLE && c_b->type == COLLIDER_BOX) {
        for (int k = 0; k < c_a->circle_count; k++) {
            resolve_single_circle_vs_box(t_a, c_a->circles[k], t_b, c_b);
        }
    } else if (c_a->type == COLLIDER_BOX && c_b->type == COLLIDER_CIRCLE) {
        for (int k = 0; k < c_b->circle_count; k++) {
            resolve_single_circle_vs_box(t_b, c_b->circles[k], t_a, c_a);
        }
    } else if (c_a->type == COLLIDER_CIRCLE && c_b->type == COLLIDER_CIRCLE) {
        for (int ka = 0; ka < c_a->circle_count; ka++) {
            for (int kb = 0; kb < c_b->circle_count; kb++) {
                resolve_single_circle_vs_circle(t_a, c_a->circles[ka], t_b, c_b->circles[kb], c_b->is_static);
            }
        }
    }
}

static void process_entity_pair(const Entity i, const Entity j) {
    if (!HAS_COMPONENT(transform, i) || !HAS_COMPONENT(collider, i)) return;
    if (!HAS_COMPONENT(transform, j) || !HAS_COMPONENT(collider, j)) return;

    Transform2D *t_a = GET_COMPONENT(transform, i);
    Collider2D *c_a = GET_COMPONENT(collider, i);

    Transform2D *t_b = GET_COMPONENT(transform, j);
    Collider2D *c_b = GET_COMPONENT(collider, j);

    if (!can_collide(c_a, c_b)) return;

    if (resolve_trigger_overlap(i, t_a, c_a, j, t_b, c_b)) return;

    resolve_physics_collision(t_a, c_a, t_b, c_b);
}

void collision_system_update(void) {
    reset_all_trigger_contacts();

    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        for (Entity j = i + 1; j < MAX_ENTITIES; j++) {
            process_entity_pair(i, j);
        }
    }
}
