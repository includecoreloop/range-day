#include "movement_system.h"
#include "../components/transform.h"
#include "../components/velocity.h"

#include "../components/humanoid_render.h"
#include "../components/player.h"

#include <math.h>

void movement_system_update(const double dt) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (HAS_COMPONENT(transform, i) && HAS_COMPONENT(velocity, i)) {
            Transform2D *t = GET_COMPONENT(transform, i);
            const Velocity *v = GET_COMPONENT(velocity, i);

            t->position.x += v->direction.x * v->speed * dt;
            t->position.y += v->direction.y * v->speed * dt;
        }
    }
}

void leg_animation_system_update(const double dt) {
    const float float_dt = (float) dt;

    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(humanoid_render, i)) continue;

        HumanoidRender *hr = GET_COMPONENT(humanoid_render, i);

        float target_amplitude = 0.0f;
        float phase_speed = 0.0f;

        bool is_normal_stance = true;
        bool is_running = false;
        bool is_walking = false;

        if (HAS_COMPONENT(player, i)) {
            const Player *p = GET_COMPONENT(player, i);
            is_normal_stance = p->stance_state == PLAYER_STANCE_NORMAL;
            is_running = p->locomotion_state == PLAYER_LOCOMOTION_RUNNING;
            is_walking = p->locomotion_state == PLAYER_LOCOMOTION_WALKING;
        } else if (HAS_COMPONENT(velocity, i)) {
            const Velocity *v = GET_COMPONENT(velocity, i);
            is_walking = v->speed > 0.1f;
        }

        if (is_normal_stance) {
            if (is_running) {
                target_amplitude = hr->run_stride_amplitude;
                phase_speed = hr->run_stride_speed;
            } else if (is_walking) {
                target_amplitude = hr->walk_stride_amplitude;
                phase_speed = hr->walk_stride_speed;
            }
        }

        if (phase_speed > 0.0f) {
            hr->leg_phase = fmodf(hr->leg_phase + phase_speed * float_dt, 2.0f * PI);
        }

        const float lerp_factor = 1.0f - expf(-15.0f * float_dt);
        hr->current_stride_offset += (target_amplitude - hr->current_stride_offset) * lerp_factor;
    }
}
