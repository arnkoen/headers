#include "../thread.h"
#include <stdio.h>

void* thread_func(void* arg) {
    int id = *(int*)arg;
    printf("Hello from thread %d!\n", id);
    mt_thread_sleep_ms(500); // Sleep for 500ms
    return NULL;
}

int main(void) {
    mt_thread threads[2];
    int ids[2] = {1, 2};

    for (int i = 0; i < 2; ++i) {
        if (mt_thread_create(&threads[i], thread_func, &ids[i]) != 0) {
            fprintf(stderr, "Failed to create thread %d\n", i);
            return 1;
        }
    }

    for (int i = 0; i < 2; ++i) {
        mt_thread_join(threads[i]);
    }

    printf("All threads finished.\n");
    return 0;
}
