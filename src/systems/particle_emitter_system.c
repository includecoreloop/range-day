#include "particle_emitter_system.h"
#include "../components/particle_emitter.h"
#include "../game/prefabs/particle_emitter_prefab.h"

void particle_emitter_system_update(const float dt) {
    for (Entity i = 0; i < MAX_ENTITIES; i++) {
        if (!HAS_COMPONENT(particle_emitter, i)) continue;

        ParticleEmitter *emitter = GET_COMPONENT(particle_emitter, i);
        int active_particles = emitter->count;

        for (int j = 0; j < emitter->count; j++) {
            Particle *p = &emitter->particles[j];

            if (p->lifetime < p->max_lifetime) {
                p->position.x += p->velocity.x * dt;
                p->position.y += p->velocity.y * dt;
                p->lifetime += dt;
            } else {
                active_particles--;
            }
        }

        if (active_particles <= 0) {
            particle_emitter_prefab_destroy(i);
        }
    }
}
