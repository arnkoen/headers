/*
If you are not sure, what this is about, have a look here:
https://floooh.github.io/2018/06/17/handles-vs-pointers.html

This header is heavily based on the handle implementation of Sepehr Taghdisian (septag@github).
License: https://github.com/septag/sx#license-bsd-2-clause

//----------------------------------------------

BSD 2-Clause License

Copyright (c) 2025, Arne König
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#ifndef HANDLE_POOL_H
#define HANDLE_POOL_H
#include <stdint.h>
#include <stdbool.h>

#if !defined(HP_ARENA_MALLOC) || !defined(HP_ARENA_FREE)
#include <stdlib.h>
#define HP_ARENA_MALLOC(x) malloc(x)
#define HP_ARENA_FREE(x) free(x)
#endif
#if !defined(HP_ASSERT)
#include <assert.h>
#define HP_ASSERT(x) assert(x)
#endif

typedef uint32_t hp_handle;
#define INVALID_HANDLE 0

typedef struct hp_pool {
    int count;
    int capacity;
    hp_handle* dense;
    int* sparse;
} hp_pool;

void hp_init(hp_pool* pool, int capacity);
void hp_destroy(hp_pool* pool);
void hp_reset(hp_pool* pool);
bool hp_is_full(const hp_pool* pool);

hp_handle hp_create_handle(hp_pool* pool);
void hp_release_handle(hp_pool* pool, hp_handle* hnd);
bool hp_valid_handle(const hp_pool* pool, hp_handle hnd);
hp_handle hp_handle_at(const hp_pool* pool, int idx);
#define hp_index(_h) (int)((_h) & HANDLE_INDEX_MASK)

#define HANDLE_GEN_BITS 14
#define HANDLE_INDEX_MASK ((1u << (32 - HANDLE_GEN_BITS)) - 1u)
#define HANDLE_GEN_MASK ((1u << HANDLE_GEN_BITS) - 1u)
#define HANDLE_GEN_SHIFT (32 - HANDLE_GEN_BITS)

#endif // HANDLE_POOL_H

#ifdef HANDLE_POOL_IMPL


#define _handle_gen(_h)   (int)(((_h) >> HANDLE_GEN_SHIFT) & HANDLE_GEN_MASK)
#define _handle_make(_g, _idx) \
    (uint32_t)((((uint32_t)(_g) & HANDLE_GEN_MASK) << HANDLE_GEN_SHIFT) | \
    ((uint32_t)(_idx) & HANDLE_INDEX_MASK))

void hp_init(hp_pool* pool, int capacity) {
    pool->dense = (hp_handle*)HP_ARENA_MALLOC(capacity * sizeof(hp_handle));
    HP_ASSERT(pool->dense);
    pool->sparse = (int*)HP_ARENA_MALLOC(capacity * sizeof(int));
    HP_ASSERT(pool->sparse);
    pool->capacity = capacity;
    hp_reset(pool);
}

void hp_reset(hp_pool* pool) {
    pool->count = 0;
    for (int i = 0; i < pool->capacity; i++) {
        pool->dense[i]  = _handle_make(0, i);
        pool->sparse[i] = -1;
    }
}

void hp_destroy(hp_pool* pool) {
    HP_ASSERT(pool);
    if (pool->dense)  HP_ARENA_FREE(pool->dense);
    if (pool->sparse) HP_ARENA_FREE(pool->sparse);
}

bool hp_is_full(const hp_pool* pool) {
    return pool->count == pool->capacity;
}

hp_handle hp_create_handle(hp_pool* pool) {
    if (pool->count < pool->capacity) {
        int index = pool->count++;
        hp_handle hnd = pool->dense[index];

        int gen = _handle_gen(hnd);
        int orig_idx = hp_index(hnd);
        hp_handle new_h = _handle_make(++gen, orig_idx);

        pool->dense[index] = new_h;
        pool->sparse[orig_idx] = index;
        return new_h;
    }
    return INVALID_HANDLE;
}

void hp_release_handle(hp_pool* pool, hp_handle* hnd) {
    HP_ASSERT(pool->count > 0);
    HP_ASSERT(hp_valid_handle(pool, *hnd));

    int idx = hp_index(*hnd);
    int dense_pos = pool->sparse[idx];
    hp_handle last_h = pool->dense[--pool->count];

    pool->dense[pool->count] = *hnd;
    pool->sparse[hp_index(last_h)] = dense_pos;
    pool->dense[dense_pos] = last_h;
    *hnd = INVALID_HANDLE;
}

bool hp_valid_handle(const hp_pool* pool, hp_handle hnd) {
    if (hnd == INVALID_HANDLE) return false;

    uint32_t idx = hp_index(hnd);
    if (idx >= (uint32_t)pool->capacity) return false;

    int dense_pos = pool->sparse[idx];
    //if (dense_pos < 0 || dense_pos >= pool->count) return false;

    return pool->dense[dense_pos] == hnd;
}

hp_handle hp_handle_at(const hp_pool* pool, int idx) {
    HP_ASSERT(idx < pool->count);
    return pool->dense[idx];
}

#endif // HANDLE_IMPL
