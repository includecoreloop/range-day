#include "particle_emitter_prefab.h"
#include "../../components/transform.h"
#include "../../components/particle_emitter.h"
#include <stdlib.h>

Entity particle_emitter_prefab_create(const Vec2 position, const Vec2 normal, const float duration) {
    const Entity emitter_entity = create_entity();

    const Transform2D transform_comp = {
        .position = position,
        .scale = {.x = 1.0f, .y = 1.0f},
        .rotation = 0.0f
    };
    ADD_COMPONENT(transform, emitter_entity, transform_comp);

    ParticleEmitter emitter_comp = {.count = 8 + rand() % 5};

    for (int i = 0; i < emitter_comp.count; i++) {
        const float spread_x = ((float) (rand() % 100) / 100.0f - 0.5f) * 2.0f;
        const float spread_y = ((float) (rand() % 100) / 100.0f - 0.5f) * 2.0f;

        const Vec2 dir = {
            .x = normal.x + spread_x * 0.5f,
            .y = normal.y + spread_y * 0.5f
        };

        const float speed = 100.0f + (float) (rand() % 100);

        emitter_comp.particles[i] = (Particle){
            .position = position,
            .velocity = {.x = dir.x * speed, .y = dir.y * speed},
            .rgba = 0xFFFFFFFF,
            .size = 3.0f,
            .lifetime = 0.0f,
            .max_lifetime = duration
        };
    }

    ADD_COMPONENT(particle_emitter, emitter_entity, emitter_comp);

    return emitter_entity;
}

void particle_emitter_prefab_destroy(const Entity emitter_entity) {
    REMOVE_COMPONENT(transform, emitter_entity);
    REMOVE_COMPONENT(particle_emitter, emitter_entity);
    destroy_entity(emitter_entity);
}
