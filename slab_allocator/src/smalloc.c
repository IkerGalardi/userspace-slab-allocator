#include "smalloc.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "slab.h"

#define PAGE_SIZE 1024 * 4

#define SMALLOC_CONFIG_DEBUG

#ifdef SMALLOC_CONFIG_DEBUG
    #define debug(...) fprintf(stderr, __VA_ARGS__)
#else
    #define debug(...)
#endif

struct mem_slab* byte_cache = NULL;
struct mem_slab* byte4_cache = NULL;
struct mem_slab* byte8_cache = NULL;
struct mem_slab* byte16_cache = NULL;

static void* allocate_and_grow_if_necessary_from_slab(struct mem_slab* slab) {
    struct mem_slab* slab_to_allocate = slab;
    void* ptr = mem_slab_alloc(slab_to_allocate);

    while(ptr == NULL) {
        debug("\t* Cache %p full\n", slab);
        if(slab_to_allocate->next == NULL) {
            slab_to_allocate->next = mem_slab_create(slab_to_allocate->size, slab_to_allocate->alignment);
            
            // If no more slabs can be created, simply return NULL, probably system OOM
            if(slab_to_allocate->next == NULL) {
                debug("\t* Could not create more caches, system probably OOM\n");
                return NULL;
            }

            debug("\t* Needed another cache, so created!\n");
        }

        slab_to_allocate = slab_to_allocate->next;
        ptr = mem_slab_alloc(slab_to_allocate);
    }

    debug("\t* Allocated pointer is %p\n", ptr);

    return ptr;
}

static struct mem_slab* is_ptr_allocated_in_pool(struct mem_slab* pool, void* ptr) {
    struct mem_slab* current_slab = pool;
    while(current_slab != NULL) {
        void* page_of_ptr = (void*)((uint64_t)ptr - ((uint64_t)ptr % PAGE_SIZE));
        debug("\t* Pointer of cache is %p\n", current_slab);

        // If the pointer is in the same page as the slab, then the pointer
        // was allocated on that slab
        if(current_slab == page_of_ptr) {
            debug("\t* Found cache!\n");
            return current_slab;
        }

        current_slab = current_slab->next;
    }

    return NULL;
}

void smalloc_initialize() {
    byte_cache = mem_slab_create(1,  0);
    assert((byte_cache   != NULL) && "Error initializing byte cache");
    debug("SMALLOC: created cache of size 1 at %p\n", byte_cache);

    byte4_cache = mem_slab_create(4,  0);
    assert((byte4_cache  != NULL) && "Error initializing 4 byte cache");
    debug("SMALLOC: created cache of size 4 at %p\n", byte4_cache);

    byte8_cache = mem_slab_create(8,  0);
    assert((byte8_cache  != NULL) && "Error initializing 8 byte cache");
    debug("SMALLOC: created cache of size 8 at %p\n", byte8_cache);

    byte16_cache = mem_slab_create(16, 0);
    assert((byte16_cache != NULL) && "Error initializing 16 byte cache");
    debug("SMALLOC: created cache of size 16 at %p\n", byte16_cache);
}

void* smalloc(int size) {
    assert((size >= 0) && "Allocation size must be bigger than 0"); 
    debug("SMALLOC: Allocation of size %i\n", size);

    if(size == 1) {
        return allocate_and_grow_if_necessary_from_slab(byte_cache);
    } else if(size <= 4) {
        debug("\t* Trying to use cache pool of size 4\n", size);
        return allocate_and_grow_if_necessary_from_slab(byte4_cache);
    } else if(size <= 8) {
        debug("\t* Trying to use cache pool of size 8\n", size);
        return allocate_and_grow_if_necessary_from_slab(byte8_cache);
    } else if(size <= 16) {
        debug("\t* Trying to use cache pool of size 16\n", size);
        return allocate_and_grow_if_necessary_from_slab(byte16_cache);
    } else {
        debug("\t* Could not use any cache :(\n", size);
        return malloc(size);
    }
}

void sfree(void* ptr) {
    assert((ptr != NULL) && "Passed pointer should be a valid pointer");

    debug("SMALLOC: freeing pointer %p\n", ptr);

    // Check if the pointer was allocated on the byte slab cache
    struct mem_slab* ptr_slab = is_ptr_allocated_in_pool(byte_cache, ptr);
    if(ptr_slab != NULL) {
        debug("\t* Pointer on byte cache pool\n");
        mem_slab_dealloc(ptr_slab, ptr);
        return;
    }

    // Check if the pointer was allocated on the 4 byte slab cache
    ptr_slab = is_ptr_allocated_in_pool(byte4_cache, ptr);
    if(ptr_slab != NULL) {
        debug("\t* Pointer on 4 byte cache pool\n");
        mem_slab_dealloc(ptr_slab, ptr);
        return;
    }

    // Check if the pointer was allocated on the 8 byte slab cache
    ptr_slab = is_ptr_allocated_in_pool(byte8_cache, ptr);
    if(ptr_slab != NULL) {
        debug("\t* Pointer on 8 byte cache pool\n");
        mem_slab_dealloc(ptr_slab, ptr);
        return;
    }

    // Check if the pointer was allocated on the 16 byte slab cache
    ptr_slab = is_ptr_allocated_in_pool(byte16_cache, ptr);
    if(ptr_slab != NULL) {
        debug("\t* Pointer on 16 byte cache pool\n");
        mem_slab_dealloc(ptr_slab, ptr);
        return;
    }

    // If not allocated on caches, the pointer was allocated with malloc so just free.
    debug("\t* Pointer not allocated on caches\n");
    free(ptr);
}