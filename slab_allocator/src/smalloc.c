#include "smalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "slab.h"
#include "slab_pool.h"
#include "malloc_integration.h"

//#define DEBUG_ASSERTS
#include "internal_assert.h"

//#define SMALLOC_CONFIG_DEBUG

#ifdef SMALLOC_CONFIG_DEBUG
    #define debug(...) fprintf(stderr, __VA_ARGS__)
#else
    #define debug(...)
#endif

#define SMALLOC_CACHE_COUNT 4
struct slab_pool pools[SMALLOC_CACHE_COUNT];

/*
 * Returns the pointer to the start of the page given a pointer.
 *
 * @param ptr: pointer to find the start of the page
 * @return: pointer to the start of the page of 'ptr'
 */
static void* get_page_pointer(void* ptr) {
    // NOTE: assumes 4k pages. maybe some way to detect 16k pages?
    return (void*)((uintptr_t)ptr & (~0xFFF));
}

void smalloc_initialize() {
    // NOTE: editing this array will change the cache configuration of smalloc
    size_t cache_sizes[SMALLOC_CACHE_COUNT] = { 8, 16, 24, 32 };

    // Initialize the caches using the configuration
    debug("SMALLOC: creating %i caches at initialization\n", SMALLOC_CACHE_COUNT);
    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        pools[i] = slab_pool_create(cache_sizes[i]);
    }
}

void* smalloc(size_t size) {
    assert((size >= 0) && "Allocation size must be bigger than 0"); 
    debug("SMALLOC: Allocation of size %i\n", size);

    // Find a suitable cache and try to allocate on it.
    // NOTE: Assumes that the chache configuration sizes are sorted.
    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        if(size <= pools[i].allocation_size) {
            return slab_pool_allocate(pools + i);
        }
    }

    // If this point is reached, means that no cache is suitable for allocating the
    // given size.
    return system_malloc(size);
}

void sfree(void* ptr) {
    assert((ptr != NULL) && "Passed pointer should be a valid pointer");

    // Fast path. If the pointer was not allocated in a slab we simply free and return.
    struct mem_slab* slab = get_page_pointer(ptr);
    if(slab->slab_magic != SLAB_MAGIC_NUMBER) {
        system_free(ptr);
        return;
    }

    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        if(slab_pool_deallocate(pools + i, ptr)) {
            // When the pool deallocation returns true the ptr was in the cache, so we can return
            // from the function.
            return;
        }
    }

    assert_not_reached();
}
