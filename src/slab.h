#ifndef _SLAB_H
#define _SLAB_H

#include <stdint.h>
#include <stddef.h>

#define SLAB_MAGIC_NUMBER 0x51ABA110CA704FFF

// TODO: add multi threaded lock stuff
struct mem_slab {
    // Magic number, should always be SLAB_MAGIC_NUMBER.
    uint64_t slab_magic;

    // Size of the allocations in the cache.
    size_t size;

    // Alignment requirements in the cache (IGNORED FOR NOW)
    int alignment;

    // Object allocation count
    int ref_count;
    int max_refs;

    // Reference to the first bufctl
    uint16_t freelist_start_index;
    
    // Reference to the last bufctl
    uint16_t freelist_end_index;

    void* freelist_buffer;
    void* allocable_buffer;

    struct mem_slab* next;
    struct mem_slab* prev;
};

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
 * Creates several slab cache structures, prepares the page and links them
 * accordingly.
 *
 * @param size: size of the cache allocations
 * @param alignment: alignment requirements for allocations in slab cache
 * @param next: the slab to add at the end
 *
 * @return: first slab
 */
struct mem_slab* mem_slab_create_several(int size, int alignment, int count, struct mem_slab* next);

/*
 * Frees the cache page.
 *
 * @param slab: slab cache to be freed
 */
void mem_slab_free(struct mem_slab* slab);

/*
 * Allocate an object in the cache.
 *
 * @param slab: slab to allocate the object in
 *
 * @return: pointer to the allocated buffer
 */
void* mem_slab_alloc(struct mem_slab* slab);

/*
 * Frees an object in the cache.
 *
 * @param slab: slab in which the object is allocated
 * @param ptr: pointer to the allocated object
 */
void  mem_slab_dealloc(struct mem_slab* slab, void* ptr);

#endif // _SLAB_H
