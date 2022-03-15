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

void test_smalloc_random_allocations() {
    const int number_of_allocations = 20;

    int allocation_sizes[] = { 4, 1, 3, 9, 8, 6, 2, 5, 7, 14, 11, 13, 19, 18, 16, 21, 15, 17 };
    void* allocations[number_of_allocations];

    // Do allocations of random sizes
    for(int i = 0; i < number_of_allocations; i++) {
        printf("TEST: allocation %i\n", i); fflush(stdout);
        int size_to_alloc = i % 18;
        allocations[i] = smalloc(size_to_alloc);
    }

    // Free the allocations because we are good boiz
    for(int i = number_of_allocations - 1; i >= 0; i--) {
        printf("TEST: freeing %i\n", i); fflush(stdout);
        sfree(allocations[i]);
    }
}

int main(int argc, char** argv) {
    smalloc_initialize();
//    printf("Basic allocations:\n");
//    test_basic_allocations();
//
//    printf("Allocating and deallocating all:\n");
//    test_allocate_deallocate_all();
//
//    printf("Fulling the cache:\n");
//    test_full_cache();
//
    //test_smalloc_basic();
    test_smalloc_random_allocations();
}
