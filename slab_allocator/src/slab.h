#ifndef _SLAB_H
#define _SLAB_H

#include <stdint.h>

// TODO: add multi threaded lock stuff
struct mem_slab {
    struct mem_slab* next;
    int size;
    int alignment;
    int ref_count;
    uint8_t data[];
};

struct slab_bufctl {
    struct slab_bufctl* next;
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
