#include "allocator.h"
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#define MIN_SPLIT_ALLOWED 4
block_header_t* free_list = NULL;


void* allocator_bump_allocator(intptr_t size) {
    void* ptr = sbrk(size);
    if (ptr == (void*) -1) {
        printf("bump_allocator failed with: %s \n",strerror(errno)); 
        return NULL;
    }
    return ptr;

}

void* allocator_my_malloc(size_t size) {
    if(size == 0) {
        // invalid input 
        return NULL;
    }
    // check already allocated free blocks before allocating more memory 
    if (free_list!=NULL) {
        block_header_t* curr = free_list;
        while (curr != NULL) {
            // already existing block logic 
            if(curr->is_free && curr->size>= size) {
                // check if split will result in a to small block
                if(curr->size >= size + sizeof(block_header_t) + MIN_SPLIT_ALLOWED) {
                    size_t remainder =  curr->size - size - sizeof(block_header_t);
                    curr->size = size;
                    curr->is_free = 0;
                    
                    block_header_t* next_header = (block_header_t*)((char*)curr + sizeof(block_header_t) + size);
                    next_header->size = remainder;
                    next_header->is_free = 1;
                    next_header->next=curr->next;

                    curr->next = next_header;
                    void* data_p = curr + 1;
                    return data_p;
                } else {
                    curr->is_free= 0;
                    return curr + 1;
                }
            }
        curr = curr->next;
        }  
    }
    void* allocated = allocator_bump_allocator(sizeof(block_header_t)+ size );
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

void allocator_free(void * ptr) {
    if (ptr == NULL){
        printf("allocator_free failed: NULL pointer \n"); 
        return;
    }
    block_header_t* header = (block_header_t*)ptr - 1; // step back sizeof(block_header_t) bytes.
    header->is_free = 1;
    // walk the list and coalese adj blocks if they have free data 
    block_header_t* curr = free_list;
    while (curr->next != NULL) {
        if(curr->is_free && curr->next->is_free) {
            curr->size = curr->size + sizeof(block_header_t) + curr->next->size;
            curr->next = curr->next->next;
        }
        else {
            curr = curr->next;
        }
    }
}
