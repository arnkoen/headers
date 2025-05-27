/*
Copyright (c) Arne Koenig 2025
Redistribution and use in source and binary forms, with or without modification, are permitted.
THIS SOFTWARE IS PROVIDED 'AS-IS', WITHOUT ANY EXPRESS OR IMPLIED WARRANTY. IN NO EVENT WILL THE AUTHORS BE HELD LIABLE FOR ANY DAMAGES ARISING FROM THE USE OF THIS SOFTWARE. 
*/

#ifndef AK_ARENA_H
#define AK_ARENA_H

#include <stddef.h> //size_t

#ifndef AK_MALLOC
#include <stdlib.h>
#define AK_MALLOC(size) malloc(size)
#endif
#ifndef AK_FREE
#define AK_FREE(ptr) free(ptr)
#endif


typedef struct ak_arena {
    char* buffer;
    size_t capacity;
    size_t offset;
} ak_arena;

int ak_arena_init(ak_arena* arena, size_t capacity);
void* ak_arena_alloc(ak_arena* arena, size_t size, size_t alignment);
void ak_arena_reset(ak_arena* arena);
void ak_arena_free(ak_arena* arena);

#endif // AK_ARENA_H

#ifdef AK_ARENA_IMPL

int ak_arena_init(ak_arena* arena, size_t capacity) {
    arena->buffer = (char*)AK_MALLOC(capacity);
    if(!arena->buffer) return 0;
    arena->capacity = capacity;
    arena->offset = 0;
    return 1;
}

static size_t _align_forward(size_t ptr, size_t align) {
    size_t mod = ptr % align;
    return mod == 0 ? ptr : ptr + (align - mod);
}

void* ak_arena_alloc(ak_arena* arena, size_t size, size_t align) {
    size_t current = (size_t)arena->buffer + arena->offset;
    size_t alignment = align == 0 ? sizeof(void*) : align;
    size_t aligned = _align_forward(current, alignment);
    size_t new_offset = aligned - (size_t)arena->buffer + size;

    if(new_offset > arena->capacity) return NULL;

    void* ret = (void*)(arena->buffer + (aligned - (size_t)arena->buffer));
    arena->offset = new_offset;
    return ret;
}

void ak_arena_reset(ak_arena* arena) {
    arena->offset = 0;
}

void ak_arena_free(ak_arena* arena) {
    AK_FREE(arena->buffer);
    arena->buffer = NULL;
    arena->capacity = 0;
    arena->offset = 0;
}

#ifdef AK_ARENA_EXAMPLE

#include <stdio.h>

int main() {
    ak_arena arena;
    if (!ak_arena_init(&arena, 1024)) {
        printf("Failed to initialize arena\n");
        return 1;
    }

    //if the alignment is 0, defaults to sizeof(void*) internally.
    int* nums = (int*)ak_arena_alloc(&arena, 10 * sizeof(int), 0);
    for (int i = 0; i < 10; ++i) {
        nums[i] = i;
    }

    for (int i = 0; i < 10; ++i) {
        printf("%d ", nums[i]);
    }
    printf("\n");

    ak_arena_free(&arena);
    return 0;
}

#endif //EXAMPLE
#endif //IMPL
