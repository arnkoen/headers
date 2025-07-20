/*
dll.h - minimal cross platform dynamic library loading

Copyright (c) Arne Koenig 2025
Redistribution and use in source and binary forms, with or without modification, are permitted.
THIS SOFTWARE IS PROVIDED 'AS-IS', WITHOUT ANY EXPRESS OR IMPLIED WARRANTY. IN NO EVENT WILL THE AUTHORS BE HELD LIABLE FOR ANY DAMAGES ARISING FROM THE USE OF THIS SOFTWARE.
*/

#ifndef DLL_H
#define DLL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
typedef HMODULE dll_handle;
#else
#include <dlfcn.h>
typedef void* dll_handle;
#endif

static inline dll_handle dll_open(const char *path) {
#if defined(_WIN32)
    return LoadLibraryA(path);
#else
    return dlopen(path, RTLD_LAZY);
#endif
}

static inline void* dll_symbol(dll_handle handle, const char *symbol) {
#if defined(_WIN32)
    return (void*)GetProcAddress(handle, symbol);
#else
    return dlsym(handle, symbol);
#endif
}

static inline void dll_close(dll_handle handle) {
#if defined(_WIN32)
    if (handle) FreeLibrary(handle);
#else
    if (handle) dlclose(handle);
#endif
}

#ifdef __cplusplus
}
#endif

#endif //DLL_H
