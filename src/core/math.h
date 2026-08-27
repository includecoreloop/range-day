#pragma once

#ifndef PI
#define PI 3.14159265358979323846f
#endif

#ifndef DEG2RAD
#define DEG2RAD (PI/180.0f)
#endif

#ifndef RAD2DEG
#define RAD2DEG (180.0f/PI)
#endif

#ifndef SQRT2
#define SQRT2 1.41421356237309504880f
#endif

typedef struct {
    float x;
    float y;
} Vec2;

static inline float math_clamp(const float value, const float min, const float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
