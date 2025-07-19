/*
Copyright (c) Arne Koenig 2025
Redistribution and use in source and binary forms, with or without modification, are permitted.
THIS SOFTWARE IS PROVIDED 'AS-IS', WITHOUT ANY EXPRESS OR IMPLIED WARRANTY. IN NO EVENT WILL THE AUTHORS BE HELD LIABLE FOR ANY DAMAGES ARISING FROM THE USE OF THIS SOFTWARE.
*/

#ifndef ARENA_H
#define ARENA_H

#include <stddef.h> //size_t
#include <stdalign.h>

#ifndef ARENA_MALLOC
#include <stdlib.h>
#define ARENA_MALLOC(size) malloc(size)
#endif
#ifndef ARENA_FREE
#define ARENA_FREE(ptr) free(ptr)
#endif


typedef struct arena_t {
    char* buffer;
    size_t capacity;
    size_t offset;
} arena_t;

int arena_init(arena_t* arena, size_t capacity);
void* arena_alloc(arena_t* arena, size_t size, size_t alignment);
void arena_reset(arena_t* arena);
void arena_free(arena_t* arena);
#define arena_alloc_type(arena, T, count) \
    (T*)arena_alloc((arena), sizeof(T) * (count), alignof(T))

#endif // ARENA_H

#ifdef ARENA_IMPL

int arena_init(arena_t* arena, size_t capacity) {
    arena->buffer = (char*)ARENA_MALLOC(capacity);
    if(!arena->buffer) return 0;
    arena->capacity = capacity;
    arena->offset = 0;
    return 1;
}

static size_t _align_forward(size_t ptr, size_t align) {
    size_t mod = ptr % align;
    return mod == 0 ? ptr : ptr + (align - mod);
}

void* arena_alloc(arena_t* arena, size_t size, size_t align) {
    size_t current = (size_t)arena->buffer + arena->offset;
    size_t alignment = align == 0 ? sizeof(void*) : align;
    size_t aligned = _align_forward(current, alignment);
    size_t new_offset = aligned - (size_t)arena->buffer + size;

    if(new_offset > arena->capacity) return NULL;

    void* ret = (void*)(arena->buffer + (aligned - (size_t)arena->buffer));
    arena->offset = new_offset;
    return ret;
}

void arena_reset(arena_t* arena) {
    arena->offset = 0;
}

void arena_free(arena_t* arena) {
    ARENA_FREE(arena->buffer);
    arena->buffer = NULL;
    arena->capacity = 0;
    arena->offset = 0;
}

#endif //IMPL
