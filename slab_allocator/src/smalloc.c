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

static void* get_page_pointer(void* ptr) {
    return (void*)((uintptr_t)ptr & (~0xFFF));
}

static void* allocate_and_grow_if_necessary(int pool_index) {
    struct mem_slab* slab_to_allocate = caches[pool_index];
    void* ptr = mem_slab_alloc(slab_to_allocate);

    int jumps = 0;
    while(ptr == NULL) {
        debug("\t* Cache %p full\n", slab);

        // At this point we traversed all the caches of the necessary size. Instead of appending
        // the cache at the end, the new cache could be inserted at the start so that next time
        // is not necessary to traverse the whole pool every time.
        if(slab_to_allocate->next == NULL) {
            struct mem_slab* old_first = caches[pool_index];
            struct mem_slab* new_first = mem_slab_create(old_first->size, 0);
            
            caches[pool_index] = new_first;
            new_first->next = old_first;

            ptr = mem_slab_alloc(new_first);

            break;
        }

        slab_to_allocate = slab_to_allocate->next;
        ptr = mem_slab_alloc(slab_to_allocate);

        jumps++;
    }

    debug("\t* Allocated pointer is %p\n", ptr);
    debug("\t* Had to do %d jumps to allocate\n", jumps);

    return ptr;
}

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
