#include "slab.h"

#include <stddef.h>
#include <assert.h>

#include <sys/mman.h>

#define PAGE_SIZE 4 * 1024

struct mem_slab* mem_slab_create(int size, int alignment) {
    // Small checks before starting
    assert((size > 0) && "Slab size must be bigger than 0");

    // TODO: move the slab struct to the end of the buffer.
    // TODO: alignment requirements are ignored right now, dont do that :(
    struct mem_slab* result = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);
    result->next  = NULL;
    result->size = size;
    result->alignment = alignment;



    return result;
}

struct mem_slab* mem_slab_free(struct mem_slab* slab) {
    assert((slab->ref_count > 0) && "Use after free of slab");

    // Decrement the reference count and if no longer referenced free 
    // the memory.
    // TODO: check if slab memory should be checked or not before freeing
    //       the page
    slab->ref_count--;
    if(slab->ref_count == 0) {
        munmap((void*)slab, PAGE_SIZE);
    }
}
