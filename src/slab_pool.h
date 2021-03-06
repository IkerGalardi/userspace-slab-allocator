#ifndef SLAB_POOL_H
#define SLAB_POOL_H

#include <stdbool.h>
#include <stddef.h>

#include "slab.h"
#include "heuristic.h"

struct slab_pool {
    struct mem_slab* list_start;
    struct mem_slab* list_end;
    
    size_t allocation_size;

    struct heuristic_params params;
    struct heuristic_data data;
};

/*
 * Initializes the cache pool and creates the linked list structure.
 *
 * @param allocation_size: size of allocations that caches will use
 */
struct slab_pool slab_pool_create(size_t allocation_size);

/*
 * Allocates a pointer given a pool of slabs. If all the slabs of the pool are full
 * the function will create another slab automatically.
 */
void* slab_pool_allocate(struct slab_pool* pool);

/*
 * Tries to deallocate a pointer. If the pointer was allocated on the pool true is returned, else
 * false is returned.
 */
void slab_pool_deallocate(struct slab_pool* pool, void* ptr);

#endif // SLAB_POOL_H
