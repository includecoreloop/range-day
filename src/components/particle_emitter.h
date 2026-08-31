#pragma once

#include "../ecs/ecs.h"
#include "../core/math.h"

typedef struct {
    Vec2 position;
    Vec2 velocity;
    uint32_t rgba;
    float size;
    float lifetime;
    float max_lifetime;
} Particle;

typedef struct {
    Particle particles[12];
    int count;
} ParticleEmitter;

COMPONENT_DEFINE(ParticleEmitter, particle_emitter);
