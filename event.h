
/*
Minimal generic event system for C.
Allows registering listeners (callbacks) and dispatching events with user data.

Copyright (c) Arne Koenig 2025
Redistribution and use in source and binary forms, with or without modification, are permitted.
THIS SOFTWARE IS PROVIDED 'AS-IS', WITHOUT ANY EXPRESS OR IMPLIED WARRANTY.
IN NO EVENT WILL THE AUTHORS BE HELD LIABLE FOR ANY DAMAGES ARISING FROM THE USE OF THIS SOFTWARE.
*/

#ifndef EVENT_H
#define EVENT_H

#include <stddef.h> // size_t

#ifndef EVENT_MAX_LISTENERS
#define EVENT_MAX_LISTENERS 8
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*event_callback)(void* user, void* data);

typedef struct event {
    event_callback listeners[EVENT_MAX_LISTENERS];
    void* user_datas[EVENT_MAX_LISTENERS];
    size_t count;
} event;

// Add a listener (returns 1 on success, 0 if full)
static inline int event_add(event* e, event_callback cb, void* user) {
    if (e->count >= EVENT_MAX_LISTENERS) return 0;
    e->listeners[e->count] = cb;
    e->user_datas[e->count] = user;
    e->count++;
    return 1;
}

// Remove a listener (by callback and user pointer)
static inline void event_remove(event* e, event_callback cb, void* user) {
    for (size_t i = 0; i < e->count; ++i) {
        if (e->listeners[i] == cb && e->user_datas[i] == user) {
            for (size_t j = i + 1; j < e->count; ++j) {
                e->listeners[j - 1] = e->listeners[j];
                e->user_datas[j - 1] = e->user_datas[j];
            }
            e->count--;
            break;
        }
    }
}

// Dispatch event to all listeners
static inline void event_dispatch(event* e, void* data) {
    for (size_t i = 0; i < e->count; ++i) {
        e->listeners[i](e->user_datas[i], data);
    }
}

#ifdef __cplusplus
}
#endif

#endif // EVENT_H
