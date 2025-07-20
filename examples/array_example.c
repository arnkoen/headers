#include "../array.h"
#include <stdio.h>

int main() {
    array_t(int) arr = {0};
    for (int i = 0; i < 10; ++i) array_push(&arr, i * 2);
    for (size_t i = 0; i < arr.len; ++i) printf("%d\n", arr.data[i]);
    array_free(&arr);
    return 0;
}
