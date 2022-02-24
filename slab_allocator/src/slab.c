#include "slab.h"

#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

#include <sys/mman.h>

#define PAGE_SIZE 4 * 1024

#define SLAB_CONFIG_DEBUG
#define SLAB_CONFIG_DEBUG_FREELIST

#define NON_EXISTANT (uint16_t)(-1)

#define SLOT_FREE 0
#define SLOT_BUSY 1

#ifdef SLAB_CONFIG_DEBUG
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

// TODO: is packed okay???
struct slab_bufctl {
    // Linked list stuff
    uint16_t next_index;
    uint16_t prev_index;

    // To note if the buffer is free: 0 if its free and 1 if not.
    uint8_t is_free;
} __attribute__((packed));

static uint16_t get_buffer_index_from_ptr(struct mem_slab* slab, void* ptr) {
    return ptr - slab->allocable_buffer - slab->size;
}

static void print_freelist_if_enabled(struct mem_slab* slab) {
#ifdef SLAB_CONFIG_DEBUG_FREELIST
    debug("\t * Freelist state:\n");

    struct slab_bufctl* bufctl_array = (struct slab_bufctl*)(slab->freelist_buffer);

    debug("\t\t * Node %i is the first in the list\n", slab->freelist_start_index);
    debug("\t\t * Node %i is the last in the list\n", slab->freelist_end_index);
    debug("\t\t * First node pointer %p\n", &(bufctl_array[slab->freelist_start_index]));
    int current_index = slab->freelist_start_index;
    while(bufctl_array[current_index].next_index != NON_EXISTANT) {
        struct slab_bufctl current_node = bufctl_array[current_index];
        debug("\t\t * Node %i, previous = %i, next = %i\n", current_index, current_node.prev_index, current_node.next_index);
        current_index = current_node.next_index;
    }

    struct slab_bufctl current_node = bufctl_array[current_index];
    debug("\t\t * Node %i, previous = %i, next = %i, free = %i\n", current_index, current_node.prev_index, current_node.next_index);
#endif // SLAB_CONFIG_DEBUG_FREELIST
}

struct mem_slab* mem_slab_create(int size, int alignment) {
    debug("sizeof(header) = %i\n", sizeof(struct mem_slab));
    debug("sizeof(bufctl) = %i\n", sizeof(struct slab_bufctl));
    debug("sizeof(buffer) = %i\n", size);

    assert((size > 0) && "Slab size must be bigger than 0");
    assert((alignment >= 0) && "Alignment must be bigger than 0");

    debug("SLAB: creating new cache...\n");
    // NOTE: important that MAP_PRIVATE or MAP_SHARED is added as flag or no valid memory is going to be returned by the kernel.
    struct mem_slab* result = (struct mem_slab*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    
    // Linux returns -1 as address when no memory is mapped. If that happens return NULL and user should take care of that.
    if(result == (void*)-1) {
        debug("\t Could not get memory from the kernel\n");
        return NULL;
    }

    debug("\t * Got pointer %p from kernel\n", result);

    result->ref_count = 0;
    result->size = size;
    result->alignment = alignment;
    result->freelist_buffer = (struct slab_bufctl*)(result + 1);

    debug("\t * Freelist buffer at %p\n", result->freelist_buffer);

    // Taking into account the next relation, we can calculate the number of buffers that can be saved on a page:
    //          sizeof(mem_slab) + num_buffers * (buff_size * sizeof(bufctl)) = PAGE_SIZE
    int num_buffers = (PAGE_SIZE - sizeof(struct mem_slab))/(size * sizeof(struct slab_bufctl));
    debug("\t * %i allocations available on this cache\n", num_buffers);

    debug("\t * Generating freelist...\n");
    // Link all the free list
    struct slab_bufctl* freelist_buffer = (struct slab_bufctl*)result->freelist_buffer;
    for(int i = 0; i < num_buffers; i++) {
        freelist_buffer[i].prev_index = i - 1;
        freelist_buffer[i].next_index = i + 1;
        freelist_buffer[i].is_free = SLOT_FREE;
    }
    result->freelist_start_index = 0;
    result->freelist_end_index = num_buffers - 1;
    freelist_buffer[result->freelist_end_index].next_index = NON_EXISTANT;

    // TODO: take into account alignment pls
    // TODO: non allocated pattern or something should be added
    result->allocable_buffer = result->freelist_buffer + num_buffers;

    debug("\t * Slots start at %p\n", result->allocable_buffer);

    debug("\t * Page ends at %p\n", ((uint8_t*)(result) + PAGE_SIZE));

    print_freelist_if_enabled(result);

    return result;
}

void mem_slab_free(struct mem_slab* slab) {
    assert((slab != NULL));
    assert((slab->ref_count == 0) && "Can't free the slab if objects are allocated");

    munmap((void*)slab, PAGE_SIZE);

    debug("SLAB: freed page at %p\n", slab);
}

void* mem_slab_alloc(struct mem_slab* slab) {
    debug("SLAB: allocation of size %i on cache %p\n", slab->size, slab);

    struct slab_bufctl* freelist_array = (struct slab_bufctl*)(slab->freelist_buffer);
    int free_index = slab->freelist_start_index;
    debug("\t * Freelist start index is %i\n", free_index);

    // If the first node of the freelist is not free then all the buffers have been allocated.
    if(freelist_array[free_index].is_free != SLOT_FREE) {
        debug("\t * Can't allocate on this cache\n");
        return NULL;
    }
    
    int free_next = freelist_array[free_index].next_index;
    debug("\t * Index %i was found free\n", free_index);
    debug("\t * Index %i is new first\n", free_next);

    // When the buffer gets allocated, the freelist node is moved to the end, this way
    // for checking if there is any available buffer we just need to check the first node
    // of the free list.
    freelist_array[slab->freelist_end_index].next_index = free_index;
    freelist_array[free_index].prev_index = slab->freelist_end_index;
    freelist_array[free_next].prev_index = NON_EXISTANT;
    slab->freelist_start_index = freelist_array[free_index].next_index;
    freelist_array[free_index].next_index = NON_EXISTANT;
    slab->freelist_end_index = free_index;

    // Important to mark it as busy.
    freelist_array[free_index].is_free = SLOT_BUSY;

    print_freelist_if_enabled(slab);

    return slab->allocable_buffer + slab->size * free_index;
}

// TODO: fill with non allocated pattern 
void mem_slab_dealloc(struct mem_slab* slab, void* ptr) {
    struct slab_bufctl* freelist_array = (struct slab_bufctl*)(slab->freelist_buffer);
    uint16_t slot_index = get_buffer_index_from_ptr(slab, ptr);
    struct slab_bufctl* tofree_prev = &(freelist_array[slot_index - 1]);
    struct slab_bufctl* tofree      = &(freelist_array[slot_index + 0]);
    struct slab_bufctl* tofree_next = &(freelist_array[slot_index + 1]);

    debug("SLAB: deallocating slot %i on cache %p\n", slot_index, slab);

    // Mark the node as free
    tofree->is_free = SLOT_FREE; 

    // The node is already the first in the list so no movement should be done.
    if(slot_index == slab->freelist_start_index) {
        return;
    }

    // NOTE: probably when moving the end node it wont work :(
    // Move the node of the slot to the start of the freelist.
    tofree_prev->next_index = tofree->next_index;
    tofree_next->prev_index = tofree->prev_index;
    tofree->next_index = slab->freelist_start_index;
    tofree->prev_index = NON_EXISTANT;
    slab->freelist_start_index = slot_index;
    slab->freelist_end_index = tofree_prev->next_index;

    debug("\t * New first in the freelist %i\n", slab->freelist_start_index);
    debug("\t * New last in the freelist %i\n", slab->freelist_end_index);

    print_freelist_if_enabled(slab);
}
