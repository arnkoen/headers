
/*
Minimal generic memory pool for fixed-size allocations.
Intended for use with arrays of unknown size (e.g., transform* in animation structs),
to avoid memory fragmentation and allow efficient iteration.

Copyright (c) Arne Koenig 2025
Redistribution and use in source and binary forms, with or without modification, are permitted.
THIS SOFTWARE IS PROVIDED 'AS-IS', WITHOUT ANY EXPRESS OR IMPLIED WARRANTY.
IN NO EVENT WILL THE AUTHORS BE HELD LIABLE FOR ANY DAMAGES ARISING FROM THE USE OF THIS SOFTWARE.
*/

#ifndef MEMPOOL_H
#define MEMPOOL_H

#include <stddef.h> // size_t
#include <stdint.h>

#if !defined(MEMPOOL_MALLOC) || !defined(MEMPOOL_FREE)
#include <stdlib.h>
#define MEMPOOL_MALLOC(sz) malloc(sz)
#define MEMPOOL_FREE(ptr) free(ptr)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mempool_t {
    void* buffer;
    size_t elem_size;
    size_t capacity;
    size_t count;
    int* free_list;
    size_t free_count;
} mempool_t;

int mempool_init(mempool_t *pool, size_t elem_size, size_t capacity);
void* mempool_alloc(mempool_t *pool);
void mempool_free(mempool_t *pool, void* ptr);
void mempool_reset(mempool_t *pool);
void mempool_destroy(mempool_t *pool);
#define mempool_alloc_type(pool, T) ((T*)mempool_alloc(pool))

#ifdef __cplusplus
}
#endif

#endif // MEMPOOL_H

#ifdef MEMPOOL_IMPL

int mempool_init(mempool_t* pool, size_t elem_size, size_t capacity) {
    pool->elem_size = elem_size;
    pool->capacity = capacity;
    pool->count = 0;
    pool->free_count = 0;
    pool->buffer = MEMPOOL_MALLOC(elem_size * capacity);
    pool->free_list = (int*)MEMPOOL_MALLOC(sizeof(int) * capacity);
    if (!pool->buffer || !pool->free_list) {
        MEMPOOL_FREE(pool->buffer);
        MEMPOOL_FREE(pool->free_list);
        pool->buffer = NULL;
        pool->free_list = NULL;
        return 0;
    }
    return 1;
}

void* mempool_alloc(mempool_t* pool) {
    int idx;
    if (pool->free_count > 0) {
        idx = pool->free_list[--pool->free_count];
    } else if (pool->count < pool->capacity) {
        idx = (int)pool->count++;
    } else {
        return NULL;
    }
    return (char*)pool->buffer + idx * pool->elem_size;
}

void mempool_free(mempool_t* pool, void* ptr) {
    int idx = (int)(((char*)ptr - (char*)pool->buffer) / pool->elem_size);
    if (idx >= 0 && (size_t)idx < pool->capacity) {
        pool->free_list[pool->free_count++] = idx;
    }
}

void mempool_reset(mempool_t* pool) {
    pool->count = 0;
    pool->free_count = 0;
}

void mempool_destroy(mempool_t* pool) {
    MEMPOOL_FREE(pool->buffer);
    MEMPOOL_FREE(pool->free_list);
    pool->buffer = NULL;
    pool->free_list = NULL;
    pool->count = pool->capacity = pool->free_count = 0;
    pool->elem_size = 0;
}

#endif // MEMPOOL_IMPL
