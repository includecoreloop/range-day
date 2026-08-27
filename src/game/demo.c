#include <stdio.h>

#include "../ecs/ecs.h"
#include "../components/timer.h"
#include "../systems/timer_system.h"

void on_timer_done(const Entity e) {
    printf(">>> Callback fired for entity %u\n", e);
}

void setup_demo_timer(const float remaining_time) {
    const Entity e = create_entity();
    const Timer t = {
        .remaining = remaining_time,
        .active = true,
        .task_name = "Training",
        .callback = on_timer_done
    };

    ADD_COMPONENT(timer, e, t);
    timer_system_add(e);
}
