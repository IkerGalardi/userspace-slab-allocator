#include "slab.h"

#include <stddef.h>
#include <assert.h>

#include <sys/mman.h>

#define PAGE_SIZE 4 * 1024

/*
 * Memory layout of cache page
 * +-----------+--------+--------+--------+--------+ 
 * |   HEADER  | BUFCTL |  DATA  | BUFCTL |  DATA  | ....
 * +-----------+--------+--------+--------+--------+ 
 *
 */

struct mem_slab* mem_slab_create(int size, int alignment) {
    // Small checks before starting
    assert((size > 0) && "Slab size must be bigger than 0");

    // TODO: move the slab struct to the end of the buffer.
    // TODO: alignment requirements are ignored right now, dont do that :(
    struct mem_slab* result = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);
    result->next  = NULL;
    result->size = size;
    result->alignment = alignment;

    const int num_slots = (PAGE_SIZE - sizeof(struct mem_slab)) / (sizeof(struct slab_bufctl) + size);

    // Fill all the bufctl structures
    struct slab_bufctl* bufctl = (struct slab_bufctl*)(result++);
    for(int i = 0; i < num_slots; i++) {
        // TODO: Fill all the bufclt linked list items and link them
    }

    return result;
}

struct mem_slab* mem_slab_free(struct mem_slab* slab) {
    // TODO: assert for slab is free?
    assert((slab->ref_count > 0) && "Use after free of slab");

    // Decrement the reference count and if no longer referenced free 
    // the memory.
    slab->ref_count--;
    if(slab->ref_count == 0) {
        munmap((void*)slab, PAGE_SIZE);
    }
}
