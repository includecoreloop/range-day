#include "timer_system.h"
#include <stdio.h>

#define MAX_ACTIVE_TIMERS MAX_ENTITIES

static Entity active_entities[MAX_ACTIVE_TIMERS];
static size_t active_count = 0;

void timer_system_add(const Entity e) {
    if (active_count >= MAX_ACTIVE_TIMERS) return;
    active_entities[active_count++] = e;
}

void timer_system_remove(const Entity e) {
    for (size_t i = 0; i < active_count; i++) {
        if (active_entities[i] == e) {
            active_entities[i] = active_entities[active_count - 1];
            active_count--;
            return;
        }
    }
}

void timer_system_update(const float dt) {
    size_t i = 0;

    while (i < active_count) {
        const Entity e = active_entities[i];
        Timer *t = GET_COMPONENT(timer, e);

        if (!t || !t->active) {
            timer_system_remove(e);

            continue;
        }

        t->remaining -= dt;

        if (t->remaining <= 0.0f) {
            t->active = false;

            if (t->task_name)
                printf("Task '%s' completed for entity %u!\n", t->task_name, e);

            if (t->callback) {
                t->callback(e);
            }

            timer_system_remove(e);

            continue;
        }

        i++;
    }
}
