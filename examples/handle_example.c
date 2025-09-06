#define HANDLE_POOL_IMPL
#include "../handle_pool.h"
#include <stdio.h>

typedef struct {
    int id;
    int width, height;
} image;

// Creates an image in the pool, initializes its fields, and returns its handle
hp_handle create_image(hp_pool* pool, image imgs[], int id, int width, int height) {
    hp_handle h = hp_create_handle(pool);
    if (h == HP_INVALID_HANDLE) return HP_INVALID_HANDLE;

    image* img = &imgs[hp_index(h)];
    img->id = id;
    img->width = width;
    img->height = height;
    return h;
}

// Releases the image handle back into the pool
void release_image(hp_pool* pool, hp_handle* hnd) {
    hp_release_handle(pool, hnd);
}

// Retrieves a pointer to an image by its handle
image* get_image(image imgs[], hp_handle h) {
    return &imgs[hp_index(h)];
}

int main() {
    // Initialize a pool for up to 100 images
    hp_pool pool = {0};
    hp_init(&pool, 100);

    // Statically allocate image storage
    image imgs[100] = {0};

    // Create and initialize the first image
    hp_handle h0 = create_image(&pool, imgs, 0, 800, 600);
    if (h0 == HP_INVALID_HANDLE) {
        fprintf(stderr, "Failed to create first image handle\n");
        return 1;
    }
    image* img0 = get_image(imgs, h0);
    printf("Image 0: handle=%u, id=%d, %dx%d\n",
           h0, img0->id, img0->width, img0->height);

    // Release the first image
    release_image(&pool, &h0);
    printf("After release, h0 = %u (should be HP_INVALID_HANDLE)\n", h0);

    // Create a second image using the same slot
    hp_handle h1 = create_image(&pool, imgs, 1, 1280, 720);
    if (h1 == HP_INVALID_HANDLE) {
        fprintf(stderr, "Failed to create second image handle\n");
        hp_destroy(&pool);
        return 1;
    }
    image* img1 = get_image(imgs, h1);
    printf("Image 1: handle=%u, id=%d, %dx%d\n",
           h1, img1->id, img1->width, img1->height);

    // Access again via get_image
    image* lookup = get_image(imgs, h1);
    printf("Lookup via handle: id=%d\n", lookup->id);

    // Clean up
    hp_destroy(&pool);
    return 0;
}
