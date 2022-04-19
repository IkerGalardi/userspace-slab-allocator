#include "smalloc.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "slab.h"

#define PAGE_SIZE 1024 * 4

//#define SMALLOC_CONFIG_DEBUG

#ifdef SMALLOC_CONFIG_DEBUG
    #define debug(...) fprintf(stderr, __VA_ARGS__)
#else
    #define debug(...)
#endif

#define SMALLOC_CACHE_COUNT 4
struct mem_slab* caches[SMALLOC_CACHE_COUNT] = { NULL, };

/*
 * Returns the pointer to the start of the page given a pointer.
 *
 * @param ptr: pointer to find the start of the page
 * @return: pointer to the start of the page of 'ptr'
 */
static void* get_page_pointer(void* ptr) {
    return (void*)((uintptr_t)ptr & (~0xFFF));
}

/*
 * Allocates a pointer given a pool of slabs. If all the slabs of the pool are full
 * the function will create another slab automatically.
 *
 * @param pool_index: index of the cache linked list in the 'caches' array
 * @return: allocated pointer
 */
static void* allocate_and_grow_if_necessary(int pool_index) {
    struct mem_slab* slab_to_allocate = caches[pool_index];
    void* ptr = mem_slab_alloc(slab_to_allocate);

    // We expect that the first cache is always free as we append new caches to the
    // start of the list and we move to the start a cache when a slot is free'd. So
    // if the first cache is not free we append a new one to the start.
    if(ptr == NULL) {
        struct mem_slab* new_slab = mem_slab_create(slab_to_allocate->size, slab_to_allocate->alignment);
        new_slab->next = caches[pool_index];
        caches[pool_index]->prev = new_slab;
        caches[pool_index] = new_slab;

        ptr = mem_slab_alloc(new_slab);
    }

    debug("\t* Allocated pointer is %p\n", ptr);

    return ptr;
}

/*
 * Finds if a pointer is allocated in a pool of slabs.
 *
 * @param pool: pointer to the first element of slab pool
 * @param ptr: pointer to find in pool
 *
 * @return: the slab where 'ptr' is allocated, NULL if 'ptr' was not allocated in the pool.
 */
static struct mem_slab* is_ptr_allocated_in_pool(struct mem_slab* pool, void* ptr) {
    struct mem_slab* current_slab = pool;
    while(current_slab != NULL) {
        debug("\t* Pointer of cache is %p\n", current_slab);

        // If the pointer is in the same page as the slab, then the pointer
        // was allocated on that slab
        if(current_slab == get_page_pointer(ptr)) {
            debug("\t* Found cache!\n");
            return current_slab;
        }

        current_slab = current_slab->next;
    }

    return NULL;
}

void smalloc_initialize() {
    // NOTE: editing this array will change the cache configuration of smalloc
    size_t cache_sizes[SMALLOC_CACHE_COUNT] = { 8, 16, 24, 32 };

    // Initialize the caches using the configuration
    debug("SMALLOC: creating %i caches at initialization\n", SMALLOC_CACHE_COUNT);
    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        caches[i] = mem_slab_create(cache_sizes[i], 0);
        assert((caches[i] != NULL) && "Error while creating cache");

        debug("\t* Created cache of size %i\n", cache_sizes[i]);
    }
}

void* smalloc(size_t size) {
    assert((size >= 0) && "Allocation size must be bigger than 0"); 
    debug("SMALLOC: Allocation of size %i\n", size);

    // Find a suitable cache and try to allocate on it.
    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        struct mem_slab* slab = caches[i];

        debug("\t* Trying to allocate on cache of size %i\n", slab->size);

        // NOTE: Assumes that the chache configuration sizes are sorted.
        if(size <= slab->size) {
            return allocate_and_grow_if_necessary(i);
        }
    }

    // If this point is reached, means that no cache is suitable for allocating the
    // given size.
    return malloc(size);
}

void sfree(void* ptr) {
    assert((ptr != NULL) && "Passed pointer should be a valid pointer");

    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        struct mem_slab* slab = is_ptr_allocated_in_pool(caches[i], ptr);

        // If the returned slab pointer is not NULL the pointer was allocated there.
        if(slab != NULL) {
            mem_slab_dealloc(slab, ptr);
            return;
        }
    }

    // If this point is reached, means that the pointer was not allocated on any
    // cache. 
    free(ptr);
}
