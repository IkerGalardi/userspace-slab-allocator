#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <slab.h>
#include <smalloc.h>

#define TEST_NUMBER_OF_FULL_ALLOCATIONS 100

void test_basic_allocations() {
    struct mem_slab* slab = mem_slab_create(sizeof(int), 0);
    if(slab == NULL) {
        fprintf(stderr, "Error: slab creation failed!\n");
        exit(1);
    }

    void* allocated[5];
    for(int i = 0; i < 5; i++) {
        printf("TEST: Allocation number %i\n", i);
        allocated[i] = mem_slab_alloc(slab);
    }

    printf("TEST: Deallocating allocation number 2\n");
    mem_slab_dealloc(slab, allocated[2]);

    allocated[2] = mem_slab_alloc(slab);

    printf("TEST: Deallocating all\n");
    for(int i = 0; i < 5; i++) {
        mem_slab_dealloc(slab, allocated[i]);
    }

    printf("TEST: Destroying cache\n");
    mem_slab_free(slab);
}

void test_allocate_deallocate_all() {
    struct mem_slab* slab = mem_slab_create(sizeof(int), 0);

    void* allocations[TEST_NUMBER_OF_FULL_ALLOCATIONS];

    // Allocate a lot
    for(int i = 0; i < TEST_NUMBER_OF_FULL_ALLOCATIONS; i++) {
        allocations[i] = mem_slab_alloc(slab);
    }

    printf("TEST: allocated %i times\n", TEST_NUMBER_OF_FULL_ALLOCATIONS);
    
    // Deallocate all
    for(int i = 0; i < TEST_NUMBER_OF_FULL_ALLOCATIONS; i++) {
        mem_slab_dealloc(slab, allocations[i]);
    }

    printf("TEST: deallocated %i times\n", TEST_NUMBER_OF_FULL_ALLOCATIONS);

    mem_slab_free(slab);
}

void test_full_cache() {
    struct mem_slab* slab = mem_slab_create(sizeof(int), 0);
    size_t alloc_number = 0;

    while(true) {
        void* allocation = mem_slab_alloc(slab);

        if(allocation == NULL)
            break;

        alloc_number++;
    }

    printf("TEST: Allocated %i times\n", alloc_number);
}

void test_smalloc_basic() {
    smalloc_initialize();

    void* alloc[4];
    for(int i = 1; i < 4; i++) {
        printf("TEST: allocation %i, size %i\n", i, i * 4);
        alloc[i] = smalloc(i * 4);
    }

    for(int i = 1; i < 4; i++) {
        printf("TEST: deallocation %i\n", i);
        sfree(alloc[i]);
    }
}

int main(int argc, char** argv) {
//    printf("Basic allocations:\n");
//    test_basic_allocations();
//
//    printf("Allocating and deallocating all:\n");
//    test_allocate_deallocate_all();
//
//    printf("Fulling the cache:\n");
//    test_full_cache();
//
    test_smalloc_basic();
}
