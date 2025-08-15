
/*
Minimal generic hashmap for C (open addressing, linear probing). WIP!

Copyright (c) 2025, Arne Koenig
Redistribution and use in source and binary forms, with or without modification, are permitted.
THIS SOFTWARE IS PROVIDED 'AS-IS', WITHOUT ANY EXPRESS OR IMPLIED WARRANTY.
IN NO EVENT WILL THE AUTHORS BE HELD LIABLE FOR ANY DAMAGES ARISING FROM THE USE OF THIS SOFTWARE.
*/

#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>
#include <stdint.h>

#if !defined(HASHMAP_MALLOC) || !defined(HASHMAP_FREE)
#include <stdlib.h>
#define HASHMAP_MALLOC(sz) malloc(sz)
#define HASHMAP_FREE(ptr) free(ptr)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t (*hashmap_hash_fn)(const void* key, size_t key_size);
typedef int (*hashmap_eq_fn)(const void* a, const void* b, size_t key_size);

typedef struct hashmap {
    void* keys;
    void* values;
    size_t key_size, value_size;
    size_t capacity, count;
    hashmap_hash_fn hash;
    hashmap_eq_fn eq;
    uint8_t* used;
} hashmap;

static inline uint32_t hashmap_default_hash(const void* key, size_t key_size) {
    // FNV-1a, have a look here for more: https://github.com/lcn2/fnv
    uint32_t h = 2166136261u;
    const uint8_t* p = (const uint8_t*)key;
    for (size_t i = 0; i < key_size; ++i) h = (h ^ p[i]) * 16777619u;
    return h;
}

static inline int hashmap_default_eq(const void* a, const void* b, size_t key_size) {
    const uint8_t* pa = (const uint8_t*)a;
    const uint8_t* pb = (const uint8_t*)b;
    for (size_t i = 0; i < key_size; ++i) if (pa[i] != pb[i]) return 0;
    return 1;
}

static inline int hashmap_init(hashmap* map, size_t key_size, size_t value_size, size_t capacity, hashmap_hash_fn hash, hashmap_eq_fn eq) {
    map->key_size = key_size;
    map->value_size = value_size;
    map->capacity = capacity;
    map->count = 0;
    map->hash = hash ? hash : hashmap_default_hash;
    map->eq = eq ? eq : hashmap_default_eq;
    map->keys = HASHMAP_MALLOC(key_size * capacity);
    map->values = HASHMAP_MALLOC(value_size * capacity);
    map->used = (uint8_t*)HASHMAP_MALLOC(capacity);
    if (!map->keys || !map->values || !map->used) {
        HASHMAP_FREE(map->keys); HASHMAP_FREE(map->values); HASHMAP_FREE(map->used);
        return 0;
    }
    for (size_t i = 0; i < capacity; ++i) map->used[i] = 0;
    return 1;
}

static inline void hashmap_free(hashmap* map) {
    HASHMAP_FREE(map->keys);
    HASHMAP_FREE(map->values);
    HASHMAP_FREE(map->used);
    map->keys = map->values = NULL;
    map->used = NULL;
    map->capacity = map->count = 0;
}

static inline int hashmap_insert(hashmap* map, const void* key, const void* value) {
    uint32_t h = map->hash(key, map->key_size);
    size_t cap = map->capacity;
    for (size_t i = 0; i < cap; ++i) {
        size_t idx = (h + i) % cap;
        if (!map->used[idx] || map->eq((char*)map->keys + idx * map->key_size, key, map->key_size)) {
            map->used[idx] = 1;
            char* kptr = (char*)map->keys + idx * map->key_size;
            char* vptr = (char*)map->values + idx * map->value_size;
            for (size_t j = 0; j < map->key_size; ++j) kptr[j] = ((char*)key)[j];
            for (size_t j = 0; j < map->value_size; ++j) vptr[j] = ((char*)value)[j];
            if (!map->used[idx]) map->count++;
            return 1;
        }
    }
    return 0;
}

static inline void* hashmap_find(hashmap* map, const void* key) {
    uint32_t h = map->hash(key, map->key_size);
    size_t cap = map->capacity;
    for (size_t i = 0; i < cap; ++i) {
        size_t idx = (h + i) % cap;
        if (!map->used[idx]) return NULL;
        if (map->eq((char*)map->keys + idx * map->key_size, key, map->key_size))
            return (char*)map->values + idx * map->value_size;
    }
    return NULL;
}

static inline int hashmap_remove(hashmap* map, const void* key) {
    uint32_t h = map->hash(key, map->key_size);
    size_t cap = map->capacity;
    for (size_t i = 0; i < cap; ++i) {
        size_t idx = (h + i) % cap;
        if (!map->used[idx]) return 0;
        if (map->eq((char*)map->keys + idx * map->key_size, key, map->key_size)) {
            map->used[idx] = 0;
            map->count--;
            return 1;
        }
    }
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif // HASHMAP_H
