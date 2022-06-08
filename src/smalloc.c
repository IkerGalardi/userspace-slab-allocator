#include "smalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "slab.h"
#include "slab_pool.h"
#include "utils.h"
#include "internal_assert.h"

#ifdef SMALLOC_CONFIG_DEBUG
    #define debug(...) fprintf(stderr, __VA_ARGS__)
#else
    #define debug(...)
#endif

#define SMALLOC_CACHE_COUNT 4
struct slab_pool pools[SMALLOC_CACHE_COUNT];

__attribute__((constructor)) void smalloc_initialize() {
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
    // NOTE: Assumes that the cache configuration sizes are sorted.
    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        if(size <= pools[i].allocation_size) {
            return slab_pool_allocate(pools + i);
        }
    }

    // If this point is reached, means that no cache is suitable for allocating the
    // given size.
    return malloc(size);
}

void* srealloc(void* ptr, size_t size) {
    // NOTE: this should not be necessary, but apparently dash tryies to srealloc(NULL)
    if(ptr == NULL) {
        return smalloc(size);
    }

    struct mem_slab* slab = get_page_pointer(ptr);

    // If the pointer was not allocated by us, then simply realloc using system's
    // allocator.
    if(slab->slab_magic != SLAB_MAGIC_NUMBER) {
        return realloc(ptr, size);
    }

    size_t allocation_size = slab->size;

    // Our pointers are already small, so there is no need to do reallocations
    // and we can simply return the provided pointer.
    if(size < allocation_size) {
        return ptr;
    }

    // As a last resort allocate a new buffer and copy the pointer data to the
    // new buffer.
    void* new_pointer = smalloc(size);
    memcpy(new_pointer, ptr, allocation_size);
    sfree(ptr);
    return new_pointer;
}

void sfree(void* ptr) {
    assert((ptr != NULL) && "Passed pointer should be a valid pointer");

    // Fast path. If the pointer was not allocated in a slab we simply free and return.
    struct mem_slab* slab = get_page_pointer(ptr);
    if(slab->slab_magic != SLAB_MAGIC_NUMBER) {
        free(ptr);
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
