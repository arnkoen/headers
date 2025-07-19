/*
Copyright (c) Arne Koenig, 2025
Redistribution and use in source and binary forms, with or without modification, are permitted.
THIS SOFTWARE IS PROVIDED 'AS-IS', WITHOUT ANY EXPRESS OR IMPLIED WARRANTY.
IN NO EVENT WILL THE AUTHORS BE HELD LIABLE FOR ANY DAMAGES ARISING FROM THE USE OF THIS SOFTWARE.
*/

#ifndef EASINGS_H
#define EASINGS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EASINGS_COSF
    #include <math.h>
    #define EASINGS_COSF cosf
#endif
#ifndef EASINGS_SINF
    #include <math.h>
    #define EASINGS_SINF sinf
#endif
#ifndef EASINGS_POWF
    #include <math.h>
    #define EASINGS_POWF powf
#endif
#ifndef EASINGS_SQRTF
    #include <math.h>
    #define EASINGS_SQRTF sqrtf
#endif
#ifndef EASINGS_M_PI
    #ifndef M_PI
        #define EASINGS_M_PI 3.14159265358979323846f
    #else
        #define EASINGS_M_PI ((float)M_PI)
    #endif
#endif

// Linear
static inline float ease_linear(float t) {
    return t;
}

// Quadratic
static inline float ease_in_quad(float t) {
    return t * t;
}
static inline float ease_out_quad(float t) {
    return t * (2.0f - t);
}
static inline float ease_in_out_quad(float t) {
    return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
}

// Cubic
static inline float ease_in_cubic(float t) {
    return t * t * t;
}
static inline float ease_out_cubic(float t) {
    t -= 1.0f;
    return t * t * t + 1.0f;
}
static inline float ease_in_out_cubic(float t) {
    return t < 0.5f ? 4.0f * t * t * t : (t - 1.0f) * (2.0f * t - 2.0f) * (2.0f * t - 2.0f) + 1.0f;
}

// Quartic
static inline float ease_in_quart(float t) {
    return t * t * t * t;
}
static inline float ease_out_quart(float t) {
    t -= 1.0f;
    return 1.0f - t * t * t * t;
}
static inline float ease_in_out_quart(float t) {
    return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - 8.0f * (--t) * t * t * t;
}

// Quintic
static inline float ease_in_quint(float t) {
    return t * t * t * t * t;
}
static inline float ease_out_quint(float t) {
    t -= 1.0f;
    return 1.0f + t * t * t * t * t;
}
static inline float ease_in_out_quint(float t) {
    return t < 0.5f ? 16.0f * t * t * t * t * t : 1.0f + 16.0f * (--t) * t * t * t * t;
}

// Sine
static inline float ease_in_sine(float t) {
    return 1.0f - EASINGS_COSF((t * EASINGS_M_PI) / 2.0f);
}
static inline float ease_out_sine(float t) {
    return EASINGS_SINF((t * EASINGS_M_PI) / 2.0f);
}
static inline float ease_in_out_sine(float t) {
    return -0.5f * (EASINGS_COSF(EASINGS_M_PI * t) - 1.0f);
}

// Exponential
static inline float ease_in_expo(float t) {
    return t == 0.0f ? 0.0f : EASINGS_POWF(2.0f, 10.0f * (t - 1.0f));
}
static inline float ease_out_expo(float t) {
    return t == 1.0f ? 1.0f : 1.0f - EASINGS_POWF(2.0f, -10.0f * t);
}
static inline float ease_in_out_expo(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    if (t < 0.5f) return 0.5f * EASINGS_POWF(2.0f, 20.0f * t - 10.0f);
    return 1.0f - 0.5f * EASINGS_POWF(2.0f, -20.0f * t + 10.0f);
}

// Circular
static inline float ease_in_circ(float t) {
    return 1.0f - EASINGS_SQRTF(1.0f - t * t);
}
static inline float ease_out_circ(float t) {
    t -= 1.0f;
    return EASINGS_SQRTF(1.0f - t * t);
}
static inline float ease_in_out_circ(float t) {
    if (t < 0.5f)
        return 0.5f * (1.0f - EASINGS_SQRTF(1.0f - 4.0f * t * t));
    t = t * 2.0f - 2.0f;
    return 0.5f * (EASINGS_SQRTF(1.0f - t * t) + 1.0f);
}

// Back
static inline float ease_in_back(float t) {
    const float s = 1.70158f;
    return t * t * ((s + 1.0f) * t - s);
}
static inline float ease_out_back(float t) {
    const float s = 1.70158f;
    t -= 1.0f;
    return t * t * ((s + 1.0f) * t + s) + 1.0f;
}
static inline float ease_in_out_back(float t) {
    float s = 1.70158f * 1.525f;
    t *= 2.0f;
    if (t < 1.0f)
        return 0.5f * (t * t * ((s + 1.0f) * t - s));
    t -= 2.0f;
    return 0.5f * (t * t * ((s + 1.0f) * t + s) + 2.0f);
}

// Elastic
static inline float ease_in_elastic(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return -EASINGS_POWF(2.0f, 10.0f * (t - 1.0f)) * EASINGS_SINF((t - 1.075f) * (2.0f * EASINGS_M_PI) / 0.3f);
}
static inline float ease_out_elastic(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    return EASINGS_POWF(2.0f, -10.0f * t) * EASINGS_SINF((t - 0.075f) * (2.0f * EASINGS_M_PI) / 0.3f) + 1.0f;
}
static inline float ease_in_out_elastic(float t) {
    if (t == 0.0f) return 0.0f;
    if (t == 1.0f) return 1.0f;
    t *= 2.0f;
    if (t < 1.0f)
        return -0.5f * EASINGS_POWF(2.0f, 10.0f * (t - 1.0f)) * EASINGS_SINF((t - 1.1125f) * (2.0f * EASINGS_M_PI) / 0.45f);
    return 0.5f * EASINGS_POWF(2.0f, -10.0f * (t - 1.0f)) * EASINGS_SINF((t - 1.1125f) * (2.0f * EASINGS_M_PI) / 0.45f) + 1.0f;
}

// Bounce
static inline float ease_out_bounce(float t) {
    if (t < (1.0f / 2.75f)) {
        return 7.5625f * t * t;
    } else if (t < (2.0f / 2.75f)) {
        t -= 1.5f / 2.75f;
        return 7.5625f * t * t + 0.75f;
    } else if (t < (2.5f / 2.75f)) {
        t -= 2.25f / 2.75f;
        return 7.5625f * t * t + 0.9375f;
    } else {
        t -= 2.625f / 2.75f;
        return 7.5625f * t * t + 0.984375f;
    }
}
static inline float ease_in_bounce(float t) {
    return 1.0f - ease_out_bounce(1.0f - t);
}
static inline float ease_in_out_bounce(float t) {
    if (t < 0.5f)
        return 0.5f * ease_in_bounce(t * 2.0f);
    return 0.5f * ease_out_bounce(t * 2.0f - 1.0f) + 0.5f;
}

#ifdef __cplusplus
}
#endif

#endif // EASINGS_H
