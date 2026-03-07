#include "allocator.h"
#ifdef ALLOCATOR_USE_UNIX
#include "visual.h"
#endif
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#ifdef ALLOCATOR_USE_UNIX
 #include <sys/mman.h>
#endif

#define MIN_SPLIT_ALLOWED 4
#define MMAP_THRESHOLD 4000
#define ALIGNMENT 8
#define MAX(a,b) ((a) > (b) ? (a) : (b))
// (size + N +1 ) & ~(N-1) where N is how we want to allign 
#define ALIGN(size) (((size) + ALIGNMENT - 1) & ~(ALIGNMENT - 1))
block_header_t* free_list = NULL;


#ifdef ALLOCATOR_USE_UNIX

void* allocator_bump_allocator(intptr_t size) {
    void* ptr = sbrk(size);

    if (ptr == (void*)-1)
        return NULL;

    return ptr;
}

#endif

#ifdef ALLOCATOR_USE_EMBEDDED

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
void* allocator_realloc(void* ptr, size_t new_size){
    if(new_size == 0) {
        // invalid input 
        return NULL;
    }
    if (ptr == NULL) {
        return allocator_malloc(new_size);
    }

    block_header_t* old_header = (block_header_t*)ptr - 1;
    char* old_data = (char*)(old_header + 1);
    if (old_header->size >= new_size) {
        // nothing to do return same pointer
        return ptr;
    }

    size_t bytes_to_copy = old_header->size;
    // look for a block that will fit the new size 
    void* new_ptr = allocator_malloc(new_size);
    if (new_ptr == NULL) {
        return NULL;   // realloc fails
    }
    block_header_t* new_block_header = (block_header_t*)new_ptr - 1;

    
    char* new_data = (char*)(new_block_header + 1);
    // copy data 
    size_t count = 0;
    while(bytes_to_copy != count) {
        *new_data++ = *old_data++;
        count++;
    }
    allocator_free(ptr);
    return new_ptr;
}

void* allocator_malloc(size_t size) {
    if(size == 0) {
        // invalid input 
        return NULL;
    }
    size = ALIGN(size);
#ifdef ALLOCATOR_USE_UNIX
    if (size > MMAP_THRESHOLD) {
        void* ptr = mmap(NULL, sizeof(block_header_t) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == (void*) -1) {
            return NULL;
        }
        block_header_t* header = (block_header_t*)ptr;
        header->is_mmap = 1;
        header->size = size;
        header->is_free = 0;
        header->next = NULL;
        return header + 1;
    }
#endif
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
    header->is_mmap = 0;
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

void* allocator_malloc_best_fit(size_t size) {
    if(size == 0) {
        // invalid input 
        return NULL;
    }
    size = ALIGN(size);
#ifdef ALLOCATOR_USE_UNIX
    if (size > MMAP_THRESHOLD) {
        void* ptr = mmap(NULL, sizeof(block_header_t) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        if (ptr == (void*) -1) {
            return NULL;
        }
        block_header_t* header = (block_header_t*)ptr;
        header->is_mmap = 1;
        header->size = size;
        header->is_free = 0;
        header->next = NULL;
        return header + 1;
    }
#endif
    if (free_list!=NULL) {
        block_header_t* curr = free_list;
        block_header_t* best_fit = NULL;
        while (curr != NULL) {
            if(curr->is_free && curr->size>= size) {
                if (curr->size == size) {
                best_fit = curr;
                break;   // perfect match
                }
                if(best_fit == NULL || curr->size < best_fit->size) {
                    best_fit=curr;
                }
            }
        curr = curr->next;
        }
        if (best_fit == NULL) {
            return NULL;
        }
        if(best_fit->size >= size + sizeof(block_header_t) + MIN_SPLIT_ALLOWED) {
            size_t remainder =  best_fit->size - size - sizeof(block_header_t);
            best_fit->size = size;
            best_fit->is_free = 0;
            
            block_header_t* next_header = (block_header_t*)((char*)best_fit + sizeof(block_header_t) + size);
            next_header->size = remainder;
            next_header->is_free = 1;
            next_header->next=best_fit->next;

            best_fit->next = next_header;
            void* data_p = best_fit + 1;
            return data_p;
        } else {
            best_fit->is_free= 0;
            return best_fit + 1;
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
    header->is_mmap = 0;
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
        return;
    }

    block_header_t* header = (block_header_t*)ptr - 1; // step back sizeof(block_header_t) bytes.
#ifdef ALLOCATOR_USE_UNIX
    if(header->is_mmap == 1) {
        int fail = munmap((void*)header, header->size + sizeof(block_header_t));
        if(fail == -1) {
            return;
        }
        return;
    }
#endif
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

void* allocator_calloc(size_t n, size_t size) {
    if (n != 0 && size > SIZE_MAX / n) return NULL;
    
    void* ptr = allocator_malloc(n*size);
    if (ptr == NULL){
        return NULL;
    }
    return memset(ptr,0,size*n);
}

void allocator_reset(void){
    if (free_list == NULL) return;
    block_header_t* curr = free_list;
    block_header_t* prev = NULL;
    while (curr != NULL) {
        block_header_t* next = curr->next;
#ifdef ALLOCATOR_USE_UNIX       
        if (curr->is_mmap) {
            munmap(curr,curr->size+ sizeof(block_header_t));
            if(prev) {
                prev->next = next;
            } else {
                free_list = next;
            }
        }
        else 
#endif
        {
            curr->is_free = 1;
            prev = curr;
        }
        
        curr = next;
    }
    curr = free_list;
    while (curr->next != NULL) {
        curr->size += sizeof(block_header_t) + curr->next->size;
        curr->next = curr->next->next;
    }
}



#ifdef ALLOCATOR_USE_UNIX

void allocator_stats() {
    // setting out how large each box should be 
    const char *title = " HEAP ";
    int title_length = strlen(title);
    int bar_width = 67;
    size_t total_size = 0;
    size_t total_free_size = 0;
    size_t block_count = 0;
    size_t free_blocks = 0;
    block_header_t* curr = free_list;
    int block_width;
    if (curr == NULL) {
        printf("default somethging");
        return;
    }
    while (curr) {
        total_size += curr->size;
        block_count++;
        
        if(curr->is_free) {
            total_free_size += curr->size;
            free_blocks++;
        }
        curr = curr->next;
    }

    // compute summary length so we can ensure the frame is wide enough
    size_t used_size   = total_size - total_free_size;
    size_t used_blocks = block_count - free_blocks;
    int fragmentation = (total_size == 0) ? 0 : (int)((total_free_size * 100) / total_size);
    char summary_buf[256];
    int summary_len = snprintf(summary_buf, sizeof(summary_buf),
        "  Blocks: %zu  Used: %zub (%zu blocks)  Free: %zub (%zu blocks)  Fragmentation: %d%%",
        block_count, used_size, used_blocks, total_free_size, free_blocks, fragmentation);

    int frame_width = bar_width + block_count + 1;
    if (frame_width < summary_len) {
        frame_width = summary_len;
        bar_width = frame_width - block_count - 1;
    }
    
    // title bar 
    int remaining_space = frame_width - title_length;
    int left_padding = remaining_space / 2;
    int right_padding = remaining_space - left_padding;
    printf(COLOR_BORDER BOX_TL RESET);
    for(int i=0; left_padding > i; i++) {
        printf(COLOR_BORDER BOX_H RESET );
    }
    printf(COLOR_BORDER "%s" RESET, title);
    for(int i=0; right_padding> i; i++) {
        printf(COLOR_BORDER BOX_H RESET );
    }
    printf(COLOR_BORDER BOX_TR RESET );
    printf("\n");
   
    curr = free_list;
    
    // bar draw logic 
    printf("%-1s", " ");
    int printed = 0;
    while(curr != NULL) {
        printf(SEP);
        printed++;
        block_width = MAX(1, (curr->size * bar_width) / total_size);  // min size if divsion end up resulting in zero size 
        printed += block_width;
        for(int i=0; block_width>i; i++) {
            if(curr->is_free) {
                printf(COLOR_FREE BLOCK_ALLOC RESET);
            } else {
                printf(COLOR_ALLOC BLOCK_ALLOC RESET);
            }
        }
        curr = curr->next;
    }
    // close under bar
    printf(SEP "\n");
    printed++;
    printf(COLOR_BORDER BOX_LT RESET);
    for(int i=0;frame_width > i; i++) {
        printf(COLOR_BORDER BOX_H RESET );
    }
    printf(COLOR_BORDER BOX_RT RESET);

    printf("\n");

    // list
    int block_number = 0;
    printf(COLOR_GOLD "  %-6s %-18s %-10s %-8s\n"RESET, "Block", "Address", "Size (B)", "Status");
    curr = free_list;
    while(curr != NULL) {
        const char* status_color = curr->is_free ? COLOR_FREE : COLOR_ALLOC;
        const char* status_str = curr->is_free ? "FREE" : "USED";
        printf(COLOR_ADDR "  %-6d %-18p %-10zu " RESET, block_number++, (void*)(curr+1), curr->size);
        printf("%s%-8s%s\n", status_color, status_str, RESET);
        curr = curr->next;
    }

    // overview panel 
    printf("\n");
    printf(COLOR_GOLD "  Blocks: " COLOR_WHITE "%zu" COLOR_GOLD 
       "  Used: " COLOR_CORAL "%zub (%zu blocks)" COLOR_GOLD
       "  Free: " COLOR_GREEN "%zub (%zu blocks)" COLOR_GOLD  
       "  Fragmentation: " COLOR_WHITE "%d%%\n" RESET,
       block_count,
       used_size, used_blocks,
       total_free_size, free_blocks,
       fragmentation);
    
    // close title 
    printf(COLOR_BORDER BOX_BL RESET);
    for(int i=0;frame_width > i; i++) {
        printf(COLOR_BORDER BOX_H RESET );
    }
    printf(COLOR_BORDER BOX_BR RESET);
    printf("\n");

}

#endif
