#include <malloc.h>
#include <errno.h>
#include <string.h>

void* bump_allocator(__intptr_t size) {
    void* ptr = srbk(size);
    if (ptr == (void*) -1) {
        printf("bump_allocator failed with: %s \n",strerror(errno)); 
        return (void*) -1;
    }
    return ptr;

}
