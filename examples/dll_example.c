#include "../dll.h"
#include <stdio.h>

typedef int(*add_func)(int, int);

int main() {
    const char* lib_path =
    #if defined(_WIN32)
        "mydll.dll";
    #else
        "./mydll.so";
    #endif

    dll_handle lib = dll_open(lib_path);
    if (!lib) {
        printf("Failed to load library: %s\n", lib_path);
        return 1;
    }

    add_func add = (add_func)dll_symbol(lib, "add");
    if (!add) {
        printf("Failed to find symbol 'add'\n");
        dll_close(lib);
        return 1;
    }

    int result = add(2, 3);
    printf("add(2, 3) = %d\n", result);

    dll_close(lib);
    return 0;
}
