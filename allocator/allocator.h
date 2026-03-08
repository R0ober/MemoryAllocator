#pragma once

#include "../common/bump.h"

/* gate mmap, allocator_stats, and other unix-only allocator code */
#ifndef ALLOCATOR_USE_UNIX
  #ifdef BUMP_USE_UNIX
    #define ALLOCATOR_USE_UNIX
  #endif
#endif

typedef struct block_header{
    size_t size;
    int is_free;
    int is_mmap;
    struct block_header* next;
}block_header_t;

void* allocator_malloc(size_t size);
void  allocator_free(void * ptr);
void* allocator_realloc(void* ptr, size_t new_size);
void* allocator_calloc(size_t n, size_t size);
void* allocator_malloc_best_fit(size_t size);
void allocator_reset(void);
#ifdef ALLOCATOR_USE_UNIX
void allocator_stats();
#endif
