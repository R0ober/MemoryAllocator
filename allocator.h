#pragma once

/* pick -DALLOCATOR_USE_UNIX or -DALLOCATOR_USE_EMBEDDED */
#if !defined(ALLOCATOR_USE_UNIX) && !defined(ALLOCATOR_USE_EMBEDDED)
  #define ALLOCATOR_USE_UNIX   /* default to unix */
#endif

#ifdef ALLOCATOR_USE_UNIX
  #include <unistd.h>
  #include <stdint.h>
#endif

#ifdef ALLOCATOR_USE_EMBEDDED
  #include <stdint.h>
  #include <stddef.h>
  #ifndef ALLOCATOR_HEAP_SIZE
    #define ALLOCATOR_HEAP_SIZE 8192
  #endif
#endif

typedef struct block_header{
    size_t size;
    int is_free;
    int is_mmap;
    struct block_header* next;
}block_header_t;

void* allocator_bump_allocator(intptr_t size);
void* allocator_malloc(size_t size);
void  allocator_free(void * ptr);
void* allocator_realloc(void* ptr, size_t new_size);
void* allocator_calloc(size_t n, size_t size);
#ifdef ALLOCATOR_USE_UNIX
void allocator_stats();
#endif
