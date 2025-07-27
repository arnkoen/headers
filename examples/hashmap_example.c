#include "../hashmap.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    int x;
    int y;
} Value;

int main() {
    hashmap map = {0};

    if (!hashmap_init(&map, sizeof(int), sizeof(Value), 16, NULL, NULL)) {
        printf("Failed to initialize hashmap\n");
        return 1;
    }

    // Insert some key-value pairs
    for (int i = 1; i <= 5; ++i) {
        Value value = { i * 10, i * 100 };
        hashmap_insert(&map, &i, &value);
    }

    // Find and print values
    for (int i = 1; i <= 5; ++i) {
        Value* val = (Value*)hashmap_find(&map, &i);
        if (val) printf("Key %d: Value {x=%d, y=%d}\n", i, val->x, val->y);
        else printf("Key %d: Not found\n", i);
    }

    // Remove a key
    int key_to_remove = 3;
    hashmap_remove(&map, &key_to_remove);
    Value* val = (Value*)hashmap_find(&map, &key_to_remove);
    printf("After removal, key %d: %s\n", key_to_remove, val ? "Found" : "Not found");

    hashmap_free(&map);
    return 0;
}
