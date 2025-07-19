#define HANDLE_POOL_IMPL
#include "../handle_pool.h"
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>


#define TEST(name) \
    do { \
        printf("Running %s... ", #name); \
        test_##name(); \
        printf("OK\n"); \
    } while (0)

void test_create_basic() {
    hp_pool pool = {0};
    hp_init(&pool, 10);

    hp_handle h0 = hp_create_handle(&pool);
    hp_handle h1 = hp_create_handle(&pool);
    assert(h0 != INVALID_HANDLE);
    assert(h1 != INVALID_HANDLE);
    assert(hp_index(h0) == 0);
    assert(hp_index(h1) == 1);
    assert(_handle_gen(h0) == 1);
    assert(_handle_gen(h1) == 1);

    hp_destroy(&pool);
}

void test_release_and_reuse() {
    hp_pool pool = {0};
    hp_init(&pool, 5);

    hp_handle h = hp_create_handle(&pool);
    int old_gen = _handle_gen(h);
    int idx = hp_index(h);
    assert(hp_valid_handle(&pool, h));

    hp_release_handle(&pool, &h);
    assert(h == INVALID_HANDLE);

    hp_handle h2 = hp_create_handle(&pool);
    assert(hp_index(h2) == idx);
    assert(_handle_gen(h2) == old_gen + 1);
    assert(hp_valid_handle(&pool, h2));

    hp_destroy(&pool);
}

void test_handle_valid() {
    hp_pool pool = {0};
    hp_init(&pool, 3);

    hp_handle h = hp_create_handle(&pool);
    assert(hp_valid_handle(&pool, h));

    hp_handle fake = (hp_handle)(0xFFFFFFFFu);
    assert(!hp_valid_handle(&pool, fake));

    hp_release_handle(&pool, &h);
    assert(!hp_valid_handle(&pool, h));

    hp_destroy(&pool);
}

void test_pool_full() {
    hp_pool pool = {0};
    hp_init(&pool, 2);

    hp_handle h0 = hp_create_handle(&pool);
    hp_handle h1 = hp_create_handle(&pool);
    assert(hp_is_full(&pool));

    hp_handle h2 = hp_create_handle(&pool);
    assert(h2 == INVALID_HANDLE);

    hp_destroy(&pool);
}

void test_reset_pool() {
    hp_pool pool = {0};
    hp_init(&pool, 4);

    hp_handle handles[4];
    for (int i = 0; i < 4; i++) {
        handles[i] = hp_create_handle(&pool);
    }
    assert(hp_is_full(&pool));

    hp_reset(&pool);
    assert(pool.count == 0);

    hp_handle h = hp_create_handle(&pool);
    assert(hp_index(h) == 0);
    assert(_handle_gen(h) == 1);

    hp_destroy(&pool);
}

int main() {
    TEST(create_basic);
    TEST(release_and_reuse);
    TEST(handle_valid);
    TEST(pool_full);
    TEST(reset_pool);

    printf("All tests passed!\n");
    return 0;
}
