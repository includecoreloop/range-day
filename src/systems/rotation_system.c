#include "rotation_system.h"
#include "../components/transform.h"
#include "../components/velocity.h"
#include "../components/orientation_intent.h"
#include <math.h>

void rotation_system_update(void) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (HAS_COMPONENT(transform, i) && HAS_COMPONENT(orientation_intent, i)) {
            Transform2D *t = GET_COMPONENT(transform, i);
            const OrientationIntent *oi = GET_COMPONENT(orientation_intent, i);

            if (oi->look_at_moving_direction && HAS_COMPONENT(velocity, i)) {
                const Velocity *v = GET_COMPONENT(velocity, i);
                const float mag = sqrtf(v->direction.x * v->direction.x + v->direction.y * v->direction.y);

                if (mag > 0.0f) {
                    const float move_angle = atan2f(v->direction.y, v->direction.x) * RAD2DEG;
                    t->rotation = move_angle + 90.0f;
                }
            } else {
                const float delta_x = oi->look_at_pos.x - t->position.x;
                const float delta_y = oi->look_at_pos.y - t->position.y;

                t->rotation = (atan2f(delta_y, delta_x) * RAD2DEG) + 90.0f;
            }
        }
    }
}
