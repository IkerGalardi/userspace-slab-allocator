#include "smalloc.h"

#include <assert.h>
#include <stdlib.h>

#include "slab.h"

struct mem_slab* byte_cache = NULL;
struct mem_slab* byte4_cache = NULL;
struct mem_slab* byte8_cache = NULL;
struct mem_slab* byte16_cache = NULL;

static void* allocate_and_grow_if_necessary_from_slab(struct mem_slab* slab) {
    struct mem_slab* slab_to_allocate = slab;
    void* ptr = mem_slab_alloc(slab_to_allocate);

    while(ptr == NULL) {
        if(slab_to_allocate->next == NULL) {
            slab_to_allocate->next = mem_slab_create(slab_to_allocate->size, slab_to_allocate->alignment);
            
            // If no more slabs can be created, simply return NULL, probably system OOM
            if(slab_to_allocate->next == NULL)
                return NULL;
        }

        slab_to_allocate = slab_to_allocate->next;
        ptr = mem_slab_alloc(slab_to_allocate);
    }

    return ptr;
}

static struct mem_slab* get_slab_pointer_is_allocated() {
    return NULL;
}

void smalloc_initialize() {
    byte_cache = mem_slab_create(1,  0);
    assert((byte_cache   != NULL) && "Error initializing byte cache");

    byte4_cache = mem_slab_create(4,  0);
    assert((byte4_cache  != NULL) && "Error initializing 4 byte cache");

    byte8_cache = mem_slab_create(8,  0);
    assert((byte8_cache  != NULL) && "Error initializing 8 byte cache");

    byte16_cache = mem_slab_create(16, 0);
    assert((byte16_cache != NULL) && "Error initializing 16 byte cache");
}

void* smalloc(int size) {
    assert((size <= 0));

    if(size == 1) {
        return allocate_and_grow_if_necessary_from_slab(byte_cache);
    } else if(size <= 4) {
        return allocate_and_grow_if_necessary_from_slab(byte4_cache);
    } else if(size <= 8) {
        return allocate_and_grow_if_necessary_from_slab(byte8_cache);
    } else if(size <= 16) {
        return allocate_and_grow_if_necessary_from_slab(byte16_cache);
    } else {
        return malloc(size);
    }
}

void sfree(void* ptr) {
    assert((ptr != NULL) && "Passed pointer should be a valid pointer");

    struct mem_slab* slab = get_slab_pointer_is_allocated(ptr);

    // If the pointer was not allocated using a slab it means that was allocated using malloc, 
    // then simply free and return.
    if(slab == NULL) {
        free(ptr);
        return;
    }

    mem_slab_dealloc(slab, ptr);

    // TODO: maybe remove the slab from the slab list?
}
