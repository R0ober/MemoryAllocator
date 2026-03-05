#pragma once

#include <unistd.h>

typedef struct block_header{
    size_t size;
    int is_free;
    struct block_header* next;
}block_header_t;

void* allocator_bump_allocator(intptr_t size);
void* allocator_malloc(size_t size);
void  allocator_free(void * ptr);
void* allocator_realloc(void* ptr, size_t new_size);
void* allocator_calloc(size_t n, size_t size);
void allocator_stats();
