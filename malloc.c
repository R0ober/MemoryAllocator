#include "malloc.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>


block_header_t* free_list = NULL;


void* bump_allocator(intptr_t size) {
    void* ptr = sbrk(size);
    if (ptr == (void*) -1) {
        printf("bump_allocator failed with: %s \n",strerror(errno)); 
        return NULL;
    }
    return ptr;

}

void* my_malloc(size_t size) {
    if(size == 0) {
        // invalid input 
        return NULL;
    }
    void* allocated = bump_allocator(sizeof(block_header_t)+ size );
    
    if (allocated == NULL) {
        // allocation failed return NULL 
        return NULL;
    }
    block_header_t* header = (block_header_t*) allocated;
    void* data_p = header + 1;
    header->is_free = 0;
    header->size = size;
    header->next = NULL;
    if (free_list==NULL) {
        free_list = header;
    } else {
        block_header_t* curr = free_list;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = header;
    }
    return data_p;
}
