#include "bump.h"

#ifdef BUMP_USE_UNIX

void* allocator_bump_allocator(intptr_t size) {
    void* ptr = sbrk(size);

    if (ptr == (void*)-1)
        return NULL;

    return ptr;
}

#endif

#ifdef BUMP_USE_EMBEDDED

static uint8_t heap[ALLOCATOR_HEAP_SIZE];
static size_t offset = 0;

void* allocator_bump_allocator(intptr_t size) {

    if (offset + size > ALLOCATOR_HEAP_SIZE)
        return NULL;

    void* ptr = &heap[offset];
    offset += size;

    return ptr;
}

#endif
