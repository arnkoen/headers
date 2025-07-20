/*
Copyright (c) Arne Koenig, 2025
Redistribution and use in source and binary forms, with or without modification, are permitted.
THIS SOFTWARE IS PROVIDED 'AS-IS', WITHOUT ANY EXPRESS OR IMPLIED WARRANTY. IN NO EVENT WILL THE AUTHORS BE HELD LIABLE FOR ANY DAMAGES ARISING FROM THE USE OF THIS SOFTWARE.

Simple dynamic array in C. Maybe not the fastest, but easy to use.

#include "array.h"
array_t(int) arr = {0};
array_push(&arr, 42);
array_free(&arr);
*/

#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h> // size_t

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ARRAY_MALLOC
#include <stdlib.h>
#define ARRAY_MALLOC(sz) malloc(sz)
#endif
#ifndef ARRAY_REALLOC
#include <stdlib.h>
#define ARRAY_REALLOC(ptr,sz) realloc(ptr,sz)
#endif
#ifndef ARRAY_FREE
#include <stdlib.h>
#define ARRAY_FREE(ptr) free(ptr)
#endif

#define array_t(T) struct { T* data; size_t len, cap; }

#define array_reserve(a, n) \
    (((n) > (a)->cap) ? array__reserve((void**)&(a)->data, &(a)->cap, sizeof(*(a)->data), (n)) : 1)

#define array_push(a, v) \
    (array_reserve((a), (a)->len+1) ? ((a)->data[(a)->len++] = (v), 1) : 0)

#define array_pop(a) ((a)->len ? (a)->data[--(a)->len] : 0)

#define array_free(a) do { ARRAY_FREE((a)->data); (a)->data = NULL; (a)->len = (a)->cap = 0; } while(0)

static inline int array__reserve(void** data, size_t* cap, size_t elem_size, size_t n) {
    if (n <= *cap) return 1;
    size_t new_cap = *cap ? *cap * 2 : 8;
    if (new_cap < n) new_cap = n;
    void* new_data = ARRAY_REALLOC(*data, new_cap * elem_size);
    if (!new_data) return 0;
    *data = new_data;
    *cap = new_cap;
    return 1;
}

#ifdef __cplusplus
}
#endif

#endif // ARRAY_H
