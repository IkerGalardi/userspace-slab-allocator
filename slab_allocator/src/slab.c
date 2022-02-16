#include "slab.h"

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include <sys/mman.h>

#define PAGE_SIZE 4 * 1024

#define DEBUG

#ifdef DEBUG
    #define debug(...) fprintf(stderr, __VA_ARGS__)
#else
    #define debug(...)
#endif

/*
 * Memory layout of cache page
 * +-----------+--------+--------+--------+--------+--------+--------+
 * |   HEADER  | BUFCTL | BUFCTL |   ...  |PADDING |  DATA  |  DATA  |
 * +-----------+--------+--------+--------+--------+--------+--------+
 *
 */

struct slab_bufctl {
    // Linked list stuff
    uint16_t next_index;
    uint16_t prev_index;

    // To note if the buffer is free: 0 if its free and 1 if not.
    uint8_t is_free;
} __attribute__((packed));

struct mem_slab* mem_slab_create(int size, int alignment) {
    debug("sizeof(header) = %i\n", sizeof(struct mem_slab));
    debug("sizeof(bufctl) = %i\n", sizeof(struct slab_bufctl));
    debug("sizeof(buffer) = %i\n", size);

    assert((size > 0) && "Slab size must be bigger than 0");
    assert((alignment >= 0) && "Alignment must be bigger than 0");

    // NOTE: important that MAP_PRIVATE or MAP_SHARED is added as flag or no valid memory is going to be returned by the kernel.
    struct mem_slab* result = (struct mem_slab*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    
    // Linux returns -1 as address when no memory is mapped. If that happens return NULL and user should take care of that.
    if(result == (void*)-1) {
        debug("Could not get memory from the kernel\n");
        return NULL;
    }

    result->ref_count = 0;
    result->size = size;
    result->alignment = alignment;
    result->freelist_buffer = (struct slab_bufctl*)(result++);

    // Taking into account the next relation, we can calculate the number of buffers that can be saved on a page:
    //          sizeof(mem_slab) + num_buffers * (buff_size * sizeof(bufctl)) = PAGE_SIZE
    int num_buffers = (PAGE_SIZE - sizeof(struct mem_slab))/(size * sizeof(struct slab_bufctl));
    debug("%i allocations available on this cache\n", num_buffers);

    // Link all the free list
    struct slab_bufctl* freelist_buffer = (struct slab_bufctl*)result->freelist_buffer;
    for(int i = 0; i < num_buffers; i++) {
        freelist_buffer[i].prev_index = i - 1;
        freelist_buffer[i].next_index = i + 1;
        freelist_buffer[i].is_free = 0;
    }
    result->freelist_start_index = 0;
    result->freelist_end_index = num_buffers - 1;

    // TODO: take into account alignment pls
    // TODO: non allocated pattern or something should be added
    result->allocable_buffer = result->freelist_buffer + num_buffers;

    return result;
}

void mem_slab_free(struct mem_slab* slab) {
    assert((slab != NULL));
    assert((slab->ref_count == 0) && "Can't free the slab if objects are allocated");

    munmap((void*)slab, PAGE_SIZE);

    debug("SLAB: freed page at %p\n", slab);
}

void* mem_slab_alloc(struct mem_slab* slab) {
    debug("ALLOCATION NOT IMPLEMENTED YET\n");
}

void  mem_slab_dealloc(struct mem_slab* slab, void* ptr) {
    debug("DEALLOCATION NOT IMPLEMENTED YET\n");
}
