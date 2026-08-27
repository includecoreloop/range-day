#include "audio_system.h"
#include "../../core/audio_bridge.h"

#include "raylib.h"
#include <stdlib.h>
#include <math.h>

AudioBridge audio_bridge = {0};

static Sound sound_pistol = {0};
static Sound sound_rifle = {0};

static Sound generate_procedural_gunshot(const float duration_seconds, const float decay_rate) {
    const int sample_rate = 44100;
    const int total_samples = (int) (sample_rate * duration_seconds);

    short *pcm_buffer = (short *) RL_MALLOC(total_samples * sizeof(short));

    for (int i = 0; i < total_samples; i++) {
        const float t = (float) i / (float) total_samples;
        const float envelope = expf(-decay_rate * t);
        const float noise = ((float) rand() / (float) RAND_MAX) * 2.0f - 1.0f;

        pcm_buffer[i] = (short) (noise * envelope * 32767.0f);
    }

    const Wave wave = {
        .frameCount = total_samples,
        .sampleRate = sample_rate,
        .sampleSize = 16,
        .channels = 1,
        .data = pcm_buffer
    };

    const Sound sound = LoadSoundFromWave(wave);
    UnloadWave(wave);

    return sound;
}

void audio_system_init(void) {
    InitAudioDevice();

    sound_pistol = generate_procedural_gunshot(0.12f, 25.0f);

    sound_rifle = generate_procedural_gunshot(0.22f, 15.0f);
    SetSoundPitch(sound_rifle, 0.75f);
}

void audio_system_update(void) {
    if (audio_bridge.pistol_shot) {
        PlaySound(sound_pistol);
        audio_bridge_consume_action(&audio_bridge.pistol_shot);
    }

    if (audio_bridge.rifle_shot) {
        PlaySound(sound_rifle);
        audio_bridge_consume_action(&audio_bridge.rifle_shot);
    }
}

void audio_system_destroy(void) {
    UnloadSound(sound_pistol);
    UnloadSound(sound_rifle);
    CloseAudioDevice();
}
