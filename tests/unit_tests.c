#include "unit.h"
#include "../allocator.h"
#include <stdio.h>
#include <string.h>


void test_alloc_returns_non_null() {
    int *arr = (int*)allocator_bump_allocator(5 * sizeof(int));
    ASSERT_PTR_NOT_NULL(arr);
}

void test_write_and_read_back() {
    int *arr = (int*)allocator_bump_allocator(5 * sizeof(int));
    ASSERT_PTR_NOT_NULL(arr);
    for (int i = 0; i < 5; ++i) arr[i] = i * 10;
    ASSERT_EQ(40, arr[4]);
}

void test_sequential_allocations_are_contiguous() {
    void *a = allocator_bump_allocator(8);
    void *b = allocator_bump_allocator(16);
    ASSERT_PTR_NOT_NULL(a);
    ASSERT_PTR_NOT_NULL(b);
    long diff = (char*)b - (char*)a;
    ASSERT_EQ(8, diff);
}

// test 1: basic alloc and write
void test_allocator_malloc_basic_alloc_and_write() {
    void *a = allocator_malloc(8);
    ASSERT_PTR_NOT_NULL(a);
    
    block_header_t *header = (block_header_t*)a - 1;
    ASSERT_EQ(8, header->size);
    ASSERT_EQ(0, header->is_free);
    
    ASSERT_EQ((char*)(header + 1), (char*)a);
}
// test 2: free and realloc same size - should reuse block
void test_allocator_free_and_realloc_same_size() {
    void *a = allocator_malloc(16);
    block_header_t *header = (block_header_t*)a - 1;
    allocator_free(a);
    void *b = allocator_malloc(16);
    ASSERT_EQ(header,(block_header_t*)b - 1);

}
// test 3: alloc 3 blocks, free middle, check coalescing
void test_allocator_alloc_four_free_middle() {
    void *a = allocator_malloc(32);
    void *b = allocator_malloc(32);
    void *c = allocator_malloc(32);
    void *d = allocator_malloc(32);
    ASSERT_PTR_NOT_NULL(a);
    ASSERT_PTR_NOT_NULL(b);
    ASSERT_PTR_NOT_NULL(c);
    ASSERT_PTR_NOT_NULL(d);

    allocator_free(b);
    allocator_free(c);
    block_header_t* head_a = (block_header_t*)a - 1;
    block_header_t* head_b = (block_header_t*)b - 1;
    block_header_t* head_d = (block_header_t*)d - 1;

    ASSERT_EQ(1,head_b->is_free);
    ASSERT_EQ(32 + sizeof(block_header_t) + 32, head_b->size); 
    ASSERT_EQ(head_a->next,head_b);
    ASSERT_EQ(head_b->next,head_d);
}
// test 4: alloc, free, alloc larger - should request new memory
void test_allocator_alloc_free_alloc_larger() {
    void *a = allocator_malloc(64);
    block_header_t* head_a = (block_header_t*)a - 1;
    allocator_free(a);
    void *b = allocator_malloc(128);
    block_header_t* head_b = (block_header_t*)b - 1;

    ASSERT_NOT_EQ(head_a,head_b);
    ASSERT_EQ(1,head_a->is_free);
    ASSERT_EQ(0,head_b->is_free);
}

void test_allocator_realloc_data_is_preserved_after_grow() {
    int* a = (int*)allocator_malloc(4 * sizeof(int));
    int* a_start = a;
    *a++= 1;
    *a++= 2;
    *a++= 3;
    *a = 4;
    int* b = allocator_realloc((void*)a_start,8*sizeof(int));
    int* b_start = b;
    ASSERT_EQ(1,*b++);
    ASSERT_EQ(2,*b++);
    ASSERT_EQ(3,*b++);
    ASSERT_EQ(4,*b++);
    ASSERT_NOT_EQ(a_start,b_start);


}
void test_allocator_realloc_in_place_case_returns_same_pointer() {
    void* a = allocator_malloc(64);
    void* b = allocator_realloc(a,32);
    ASSERT_EQ(a,b);
}
void test_allocator_realloc_zero_size() {
    void* a = allocator_malloc(128);
    void* b = allocator_realloc(a,0);
    ASSERT_EQ(NULL,b);
}
void test_allocator_realloc_NULL_size_behaves_like_malloc() {
    void* a = allocator_malloc(128);
    void* c = allocator_realloc(NULL,32);
    ASSERT_NOT_EQ(NULL,c);
    ASSERT_EQ(((block_header_t*)c - 1)->size,32);
    // used to test 
    typedef struct three_two_byte_struct {
    long a; // 8 bytes 
    long b;
    long c;
    long d;
    }three_two_byte_struct_t;
    // this will crash program if it doesnt work as expected 
    three_two_byte_struct_t* test = (three_two_byte_struct_t*)c;
    test->a = 1;
    test->b = 2;
    test->c = 3;
    test->d = 4;
    ASSERT_EQ(test->a,1);
    ASSERT_EQ(test->b,2);
    ASSERT_EQ(test->c,3);
    ASSERT_EQ(test->d,4);

}
void test_allocator_calloc() {
    void* ptr = allocator_calloc(20,8); // 20 objects 8 bytes
    ASSERT_NOT_EQ(ptr,NULL);
    int count = 0;
    int size = 20 * 8; 
    char* objt = (char*)ptr; 
    for(int i = 0; i< size; i++) {
        if(*objt++==0) {
            count++;
        }
    }
    ASSERT_EQ(count,20*8);
}
void test_allocator_malloc_large_allocation() {
    void* ptr = allocator_malloc(5000);
    ASSERT_NOT_EQ(ptr,NULL);
    unsigned char*  chars = (unsigned char*)ptr;
    for(int i =0; i<5000; i++) {
        chars[i]=i % 256;
    }
    for(int i=0; i<5000; i++) {
        ASSERT_EQ(i % 256,chars[i]);
    }
}

void test_allocator_malloc_large_allocation_doesnt_appear_in_free_list() {
    
    void* ptr = allocator_malloc(5000);
    block_header_t* header = (block_header_t*)ptr-1;
    block_header_t* curr = free_list;
    while(curr != NULL) {
        ASSERT_NOT_EQ(header,curr);
        curr = curr->next;
    }
}

int main(void) {
    
    RUN_TEST(test_alloc_returns_non_null);
    RUN_TEST(test_write_and_read_back);
    RUN_TEST(test_sequential_allocations_are_contiguous);
    RUN_TEST(test_allocator_malloc_basic_alloc_and_write);
    RUN_TEST(test_allocator_free_and_realloc_same_size);
    RUN_TEST(test_allocator_alloc_four_free_middle);
    RUN_TEST(test_allocator_alloc_free_alloc_larger);
    RUN_TEST(test_allocator_realloc_data_is_preserved_after_grow);
    RUN_TEST(test_allocator_realloc_in_place_case_returns_same_pointer);
    RUN_TEST(test_allocator_realloc_NULL_size_behaves_like_malloc);
    RUN_TEST(test_allocator_realloc_zero_size);
    RUN_TEST(test_allocator_calloc);
    RUN_TEST(test_allocator_malloc_large_allocation);
    RUN_TEST(test_allocator_malloc_large_allocation_doesnt_appear_in_free_list);
    //printf("size: %ld",sizeof(block_header_t));
    allocator_stats();
    return SU_report_and_return();
    
    
}
