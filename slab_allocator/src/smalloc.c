#include "smalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "slab.h"
#include "cache_pool.h"
#include "internal_assert.h"

#define PAGE_SIZE 1024 * 4

//#define SMALLOC_CONFIG_DEBUG

#ifdef SMALLOC_CONFIG_DEBUG
    #define debug(...) fprintf(stderr, __VA_ARGS__)
#else
    #define debug(...)
#endif

#define SMALLOC_CACHE_COUNT 4
struct slab_pool pools[SMALLOC_CACHE_COUNT];

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
    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        // NOTE: Assumes that the chache configuration sizes are sorted.
        if(size <= pools[i].allocation_size) {
            // TODO: error checking. The slab pool can return NULL when out of memory.
            return slab_pool_allocate(pools + i);
        }
    }

    // If this point is reached, means that no cache is suitable for allocating the
    // given size.
    return malloc(size);
}

void sfree(void* ptr) {
    assert((ptr != NULL) && "Passed pointer should be a valid pointer");

    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        if(slab_pool_deallocate(pools + i, ptr)) {
            // When the pool deallocation returns true the ptr was in the cache, so we can return
            // from the function.
            return;
        }
    }

    // If this point is reached, means that the pointer was not allocated on any
    // cache. 
    free(ptr);
}
