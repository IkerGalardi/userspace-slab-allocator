#include "smalloc.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "slab.h"
#include "slab_pool.h"
#include "utils.h"
#include "internal_assert.h"

#define SMALLOC_CACHE_COUNT 6
struct slab_pool pools[SMALLOC_CACHE_COUNT];

__attribute__((constructor)) void smalloc_initialize() {
    // NOTE: editing this array will change the cache configuration of smalloc
    size_t cache_sizes[SMALLOC_CACHE_COUNT] = { 8, 16, 32, 64, 128, 256 };

    // Initialize the caches using the configuration
    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        pools[i] = slab_pool_create(cache_sizes[i]);
    }
}

void* smalloc(size_t size) {
    assert(size != 0); 

    // NOTE: Assumes that the cache configuration sizes are sorted.
    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        if(size <= pools[i].allocation_size) {
            return slab_pool_allocate(pools + i);
        }
    }

    return malloc(size);
}

void* srealloc(void* ptr, size_t size) {
    // NOTE: this should not be necessary, but apparently dash tryies to srealloc(NULL)
    if(ptr == NULL) {
        return smalloc(size);
    }

    struct mem_slab* slab = get_page_pointer(ptr);

    if(slab->slab_magic != SLAB_MAGIC_NUMBER) {
        return realloc(ptr, size);
    }

    // We don't try to retrieve a smaller buffer as the slab allocated buffers are already
    // pretty small and the dealloc/alloc operations could slow down things unecessarily.
    size_t allocation_size = slab->size;
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
    // Fast path. If the pointer was not allocated in a slab we simply free and return.
    struct mem_slab* slab = get_page_pointer(ptr);
    if(slab->slab_magic != SLAB_MAGIC_NUMBER) {
        free(ptr);
        return;
    }

    for(int i = 0; i < SMALLOC_CACHE_COUNT; i++) {
        if(pools[i].allocation_size == slab->size) {
            slab_pool_deallocate(pools + i, ptr);
            break;
        }
    }
}
