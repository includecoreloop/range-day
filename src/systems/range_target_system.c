#include "range_target_system.h"
#include "../components/range_target.h"
#include "../components/collider.h"

void range_target_system_update(const float dt) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(range_target, i)) continue;

        RangeTarget *target = GET_COMPONENT(range_target, i);
        Collider2D *col = GET_COMPONENT(collider, i);

        switch (target->state) {
            case RANGE_TARGET_STATE_PENDING:
                if (target->spawn_delay > 0.0f) {
                    target->spawn_delay -= dt;
                    if (target->spawn_delay <= 0.0f) {
                        target->spawn_delay = 0.0f;
                        target->state = RANGE_TARGET_STATE_VISIBLE;
                        target->active_timer = target->active_duration;
                        col->is_active = true;
                    }
                }

                break;

            case RANGE_TARGET_STATE_VISIBLE:
                if (target->active_timer > 0.0f) {
                    target->active_timer -= dt;
                    if (target->active_timer <= 0.0f) {
                        target->active_timer = 0.0f;
                        target->state = RANGE_TARGET_STATE_EXPIRED;
                        col->is_active = false;
                    }
                }

                break;
            default: break;
        }
    }
}
