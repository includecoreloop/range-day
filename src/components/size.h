#pragma once

#include "../ecs/ecs.h"

typedef struct {
    float width;
    float height;
} Size2D;

COMPONENT_DEFINE(Size2D, size);
