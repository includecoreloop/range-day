#pragma once

#include "../ecs/ecs.h"

typedef enum {
    RANGE_TARGET_STATE_PENDING,
    RANGE_TARGET_STATE_VISIBLE,
    RANGE_TARGET_STATE_DESTROYED,
    RANGE_TARGET_STATE_EXPIRED,
} RangeTargetState;

typedef enum {
    RANGE_TARGET_TYPE_LIGHT,
    RANGE_TARGET_TYPE_HEAVY,
} RangeTargetType;

typedef struct {
    RangeTargetState state;
    RangeTargetType type;

    float spawn_delay;
    float active_timer;
    float active_duration;
} RangeTarget;

COMPONENT_DEFINE(RangeTarget, range_target);

static inline void range_target_init(RangeTarget *rt) {
    rt->state = RANGE_TARGET_STATE_PENDING;
}

static inline void range_target_deploy(RangeTarget *rt) {
    rt->state = RANGE_TARGET_STATE_VISIBLE;
}

static inline void range_target_destroy(RangeTarget *rt) {
    rt->state = RANGE_TARGET_STATE_DESTROYED;
}

static inline void range_target_expire(RangeTarget *rt) {
    rt->state = RANGE_TARGET_STATE_EXPIRED;
}
