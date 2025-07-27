
#define MEMPOOL_IMPL
#include "../mempool.h"
#include <stdio.h>

typedef struct {
    float x, y, z;
} transform;

typedef struct {
    float timestamp;
    transform t;
} keyframe;

typedef struct {
    int num_keyframes;
    keyframe* keyframes;
} animation;

#define MAX_ANIMATIONS 2
#define MAX_KEYFRAMES 8

int main() {
    mempool_t pool;
    if (!mempool_init(&pool, sizeof(keyframe), MAX_ANIMATIONS * MAX_KEYFRAMES)) {
        printf("Failed to initialize mempool_t\n");
        return 1;
    }

    animation anims[MAX_ANIMATIONS] = {0};

    // Allocate keyframes for each animation
    for (int i = 0; i < MAX_ANIMATIONS; ++i) {
        anims[i].num_keyframes = 4 + i; // variable size
        anims[i].keyframes = mempool_alloc_type(&pool, keyframe);
        if (!anims[i].keyframes) {
            printf("Failed to allocate keyframes for animation %d\n", i);
            continue;
        }
        // Allocate additional keyframes for this animation.
        for (int j = 1; j < anims[i].num_keyframes; ++j) {
            keyframe* kf = mempool_alloc_type(&pool, keyframe);
            if (!kf) {
                printf("Pool full at animation %d, keyframe %d\n", i, j);
                break;
            }
        }
    }

    // Fill keyframes with sample data.
    for (int i = 0; i < MAX_ANIMATIONS; ++i) {
        printf("Animation %d:\n", i);
        for (int j = 0; j < anims[i].num_keyframes; ++j) {
            keyframe* kf = (keyframe*)(anims[i].keyframes + j);
            kf->timestamp = (float)j * 0.5f;
            kf->t.x = (float)i;
            kf->t.y = (float)j;
            kf->t.z = (float)(i + j);
            printf("  keyframe[%d] = t=%.2f, {%.1f, %.1f, %.1f}\n", j, kf->timestamp, kf->t.x, kf->t.y, kf->t.z);
        }
    }

    // Simulate playing the animations.
    float play_time = 0.0f;
    float duration = 2.0f;
    float dt = 0.25f;
    printf("\nSimulating animation playback:\n");
    while (play_time < duration) {
        for (int i = 0; i < MAX_ANIMATIONS; ++i) {
            int idx = 0;
            for (int j = 0; j < anims[i].num_keyframes; ++j) {
                if (anims[i].keyframes[j].timestamp <= play_time) {
                    idx = j;
                }
            }
            keyframe* kf = &anims[i].keyframes[idx];
            printf("  t=%.2f: anim %d at keyframe %d (t=%.2f, pos={%.1f,%.1f,%.1f})\n",
                play_time, i, idx, kf->timestamp, kf->t.x, kf->t.y, kf->t.z);
        }
        play_time += dt;
    }

    // Free all keyframes in reverse order.
    for (int i = MAX_ANIMATIONS - 1; i >= 0; --i) {
        for (int j = anims[i].num_keyframes - 1; j >= 0; --j) {
            keyframe* kf = (keyframe*)(anims[i].keyframes + j);
            mempool_free(&pool, kf);
        }
    }

    mempool_destroy(&pool);
    return 0;
}
