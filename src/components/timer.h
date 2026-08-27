#pragma once

#include "../ecs/ecs.h"
#include <stdbool.h>

typedef void (*TimerCallback)(Entity e);

typedef struct {
    float remaining;
    bool active;
    const char *task_name;
    TimerCallback callback;
} Timer;

COMPONENT_DEFINE(Timer, timer);
