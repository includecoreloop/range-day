#pragma once

#include <stdbool.h>

typedef struct {
    bool pistol_shot;
    bool rifle_shot;
} AudioBridge;

extern AudioBridge audio_bridge;

static inline void audio_bridge_consume_action(bool *audio) {
    *audio = false;
}
