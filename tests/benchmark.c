#include <time.h>
#include <stdio.h>
#include "../allocator.h"

extern block_header_t* free_list;

#define NUM_BLOCKS 20

// holes are different sizes
static size_t setup_sizes[NUM_BLOCKS] = {
    64, 16, 256, 16, 128, 16, 32, 16, 512, 16,
    96, 16, 48,  16, 200, 16, 80, 16, 160, 16,
};
// odd indices (all 16-byte) act as seperators 
// even indices get freed to create holes of: 64,256,128,32,512,96,48,200,80,160

// requests that go through the fragmented list
#define NUM_REQUESTS 8
static size_t request_sizes[NUM_REQUESTS] = {50, 120, 30, 200, 60, 40, 100, 24};

static void build_fragmented_heap(void* ptrs[NUM_BLOCKS], void* (*alloc)(size_t)) {
    for (int i = 0; i < NUM_BLOCKS; i++)
        ptrs[i] = alloc(setup_sizes[i]);
    // free even indices
    for (int i = 0; i < NUM_BLOCKS; i += 2)
        allocator_free(ptrs[i]);
}

int main(void) {
    void* ptrs[NUM_BLOCKS];
    void* req[NUM_REQUESTS];

    // First Fit
    clock_t start_ff = clock();
    for (int i = 0; i < 1000000; i++) {
        build_fragmented_heap(ptrs, allocator_malloc);
        for (int j = 0; j < NUM_REQUESTS; j++)
            req[j] = allocator_malloc(request_sizes[j]);
        for (int j = 0; j < NUM_REQUESTS; j++)
            allocator_free(req[j]);
        // cleanup separators
        for (int i = 1; i < NUM_BLOCKS; i += 2)
            allocator_free(ptrs[i]);
    }
    clock_t end_ff = clock();

    // snapshot leave heap fragmented
    build_fragmented_heap(ptrs, allocator_malloc);
    for (int j = 0; j < NUM_REQUESTS; j++)
        req[j] = allocator_malloc(request_sizes[j]);

    printf("\nFirst Fit \n");
    allocator_stats();
    allocator_reset();

    // Best Fit 
    clock_t start_bf = clock();
    for (int i = 0; i < 1000000; i++) {
        build_fragmented_heap(ptrs, allocator_malloc_best_fit);
        for (int j = 0; j < NUM_REQUESTS; j++)
            req[j] = allocator_malloc_best_fit(request_sizes[j]);
        for (int j = 0; j < NUM_REQUESTS; j++)
            allocator_free(req[j]);
        for (int i = 1; i < NUM_BLOCKS; i += 2)
            allocator_free(ptrs[i]);
    }
    clock_t end_bf = clock();

    // snapshot round
    build_fragmented_heap(ptrs, allocator_malloc_best_fit);
    for (int j = 0; j < NUM_REQUESTS; j++)
        req[j] = allocator_malloc_best_fit(request_sizes[j]);

    printf("\nBest Fit \n");
    allocator_stats();

    double time_ff = ((double)(end_ff - start_ff)) / CLOCKS_PER_SEC;
    double time_bf = ((double)(end_bf - start_bf)) / CLOCKS_PER_SEC;

    printf("First fit time(sec): %f  Best fit time(sec) %f: \n",time_ff,time_bf);

    return 0;
}
