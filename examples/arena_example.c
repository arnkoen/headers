#define ARENA_IMPL
#include "../arena.h"
#include <stdio.h>

int main() {
    arena arena;
    if (!arena_init(&arena, 1024)) {
        printf("Failed to initialize arena\n");
        return 1;
    }

    int* ints = arena_alloc_type(&arena, int, 10);
    for (int i = 0; i < 10; ++i) {
        ints[i] = i;
    }

    float* floats = arena_alloc_type(&arena, float, 10);
    for (int i = 0; i < 10; ++i) {
        floats[i] = (float)i;
    }

    for (int i = 0; i < 10; ++i) {
        printf("%d ", ints[i]);
    }
    printf("\n");

    for (int i = 0; i < 10; ++i) {
        printf("%f ", floats[i]);
    }
    printf("\n");

    arena_free(&arena);
    return 0;
}
