#ifndef _SLAB_H
#define _SLAB_H

#include <stdint.h>

// TODO: add multi threaded lock stuff
struct mem_slab {
    // Size of the allocations in the cache.
    int size;

    // Alignment requirements in the cache (IGNORED FOR NOW)
    int alignment;

    // Object allocation count
    int ref_count;

    // Reference to the first bufctl
    struct slab_bufctl* freelist_start;

    // Reference to the last bufctl
    struct slab_bufctl* freelist_end;
};

struct slab_bufctl {
    // Next bufctl in the free list
    struct slab_bufctl* next;
    
    // Previous bufctl in the free list
    struct slab_bufctl* prev;

    // To note if the buffer is free: 0 if its free and 1 if not.
    uint8_t is_free;
};

// TODO: check if constructors and destructors are needed for userspace

/*
 * Creates a slab cache structure and prepares the page.
 *
 * @param size: size of the cache allocations
 * @param alignment: alignment requirements for allocations in slab cache
 *
 * @return: cache structure used for allocations. NULL if can't get memory
 *          from kernel.
 */
struct mem_slab* mem_slab_create(int size, int alignment);

/*
 * Frees the cache page.
 *
 * @param slab: slab cache to be freed
 */
void mem_slab_free(struct mem_slab* slab);

#endif // _SLAB_H
