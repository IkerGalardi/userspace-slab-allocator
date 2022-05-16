#include "slab.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/mman.h>

#include "internal_assert.h"

#define SLAB_PAGE_SIZE sysconf(_SC_PAGESIZE)

//#define SLAB_CONFIG_DEBUG
//#define SLAB_CONFIG_DEBUG_FREELIST
//#define SLAB_CONFIG_DEBUG_PARANOID_ASSERTS

#define NON_EXISTANT (uint16_t)(-1)

// State in which a slot inside a slab can be.
#define SLOT_FREE 0
#define SLOT_BUSY 1

#ifdef SLAB_CONFIG_DEBUG
    #define debug(...) fprintf(stderr, __VA_ARGS__); fflush(stderr)
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
    uint16_t next_index;
    uint16_t prev_index;

    // To note if the buffer is free: can be SLOT_FREE or SLOT_BUSY.
    uint8_t is_free;
} __attribute__((packed));

/*
 * Returns the index of the buffer in the slab->allocable_buffer.
 */
static inline uint16_t get_buffer_index_from_ptr(struct mem_slab* slab, void* ptr) {
    // If the pointer is calculated with the next formula, the index should be calculated like the return statement.
    //      ptr = allocable_buffer + slab->size * index;
    //                        |
    //                        v
    //      index = (ptr - allocable_buffer) / (slab->size)

    return (ptr - slab->allocable_buffer) / (slab->size);
}

/* 
 * Returns true if the pointer specified is in the page specified. False otherwise.
 */
static inline bool is_ptr_in_page(const void* page, const void* ptr) {
    const void* page_end = (const void*)((const uint8_t*)page + SLAB_PAGE_SIZE);
    return (ptr > page) && (page_end > ptr);
}

/*
 * Prints the freelist state if SLAB_CONFIG_DEBUG_FREELIST is define.
 */
static void print_freelist(struct mem_slab* slab) {
    debug("\t * Freelist state:\n");

    struct slab_bufctl* bufctl_array = (struct slab_bufctl*)(slab->freelist_buffer);

    debug("\t\t * Node %i is the first in the list\n", slab->freelist_start_index);
    debug("\t\t * Node %i is the last in the list\n", slab->freelist_end_index);
    debug("\t\t * First node pointer %p\n", &(bufctl_array[slab->freelist_start_index]));
    int current_index = slab->freelist_start_index;
    while(bufctl_array[current_index].next_index != NON_EXISTANT) {
        struct slab_bufctl current_node = bufctl_array[current_index];
        debug("\t\t * Node %i, previous = %i, next = %i, free = %i\n", 
                current_index, current_node.prev_index, current_node.next_index, current_node.is_free);
        current_index = current_node.next_index;
    }

    struct slab_bufctl current_node = bufctl_array[current_index];
    debug("\t\t * Node %i, previous = %i, next = %i, free = %i\n", 
            current_index, current_node.prev_index, current_node.next_index, current_node.is_free);
}

static size_t get_freelist_size(struct mem_slab* slab) {
    int count = 0;

    int current_index = slab->freelist_start_index;
    struct slab_bufctl* bufctl_array = (struct slab_bufctl*)(slab->freelist_buffer);
    while(bufctl_array[current_index].next_index != NON_EXISTANT) {
        current_index = bufctl_array[current_index].next_index;
        count++;
    }
    
    return count;
}

struct mem_slab* mem_slab_create(int size, int alignment) {
    assert((size > 0) && "Slab size must be bigger than 0");
    assert((alignment >= 0) && "Alignment must be bigger than 0");

    debug("SLAB: creating new cache...\n");
    struct mem_slab* result = (struct mem_slab*)mmap(NULL, SLAB_PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    
    // Linux returns -1 as address when no memory is mapped. If that happens return NULL and user should take care of that.
    if(result == (void*)-1) {
        debug("\t Could not get memory from the kernel\n");
        return NULL;
    }

    debug("\t * Got pointer %p from kernel\n", result);

    result->slab_magic = SLAB_MAGIC_NUMBER;
    result->ref_count = 0;
    result->size = size;
    result->alignment = alignment;
    result->freelist_buffer = (struct slab_bufctl*)(result + 1);
    result->next = NULL;
    result->prev = NULL;

    debug("\t * Freelist buffer at %p\n", result->freelist_buffer);

    // Taking into account the next relation, we can calculate the number of buffers that can be saved on a page:
    //          sizeof(mem_slab) + num_buffers * (buff_size * sizeof(bufctl)) = SLAB_PAGE_SIZE
    int num_buffers = (SLAB_PAGE_SIZE - sizeof(struct mem_slab))/(size * sizeof(struct slab_bufctl));
    debug("\t * %i allocations available on this cache\n", num_buffers);


    // Link all the free list
    // TODO: maybe can avoid linking all the freelist at the start and do it with each allocation. Investigate that.
    struct slab_bufctl* freelist_buffer = (struct slab_bufctl*)result->freelist_buffer;
    for(int i = 0; i < num_buffers; i++) {
        freelist_buffer[i].prev_index = i - 1;
        freelist_buffer[i].next_index = i + 1;
        freelist_buffer[i].is_free = SLOT_FREE;
    }
    result->freelist_start_index = 0;
    result->freelist_end_index = num_buffers - 1;
    freelist_buffer[result->freelist_start_index].prev_index = NON_EXISTANT;
    freelist_buffer[result->freelist_end_index].next_index = NON_EXISTANT;

    result->max_refs = num_buffers;

    debug("\t * Freelist generated\n");

    // TODO: take into account alignment pls
    // TODO: non allocated pattern or something should be added
    result->allocable_buffer = result->freelist_buffer + num_buffers;

    debug("\t * Slots start at %p\n", result->allocable_buffer);
    debug("\t * Page ends at %p\n", ((uint8_t*)(result) + SLAB_PAGE_SIZE));

#ifdef SLAB_CONFIG_DEBUG_FREELIST
    print_freelist(result);
#endif // SLAB_CONFIG_DEBUG_FREELIST

    return result;
}

void mem_slab_free(struct mem_slab* slab) {
    assert((slab != NULL));
    assert((slab->ref_count == 0) && "Can't free the slab if objects are allocated");

    munmap((void*)slab, SLAB_PAGE_SIZE);

    debug("SLAB: freed page at %p\n", slab);
}

void* mem_slab_alloc(struct mem_slab* slab) {
    // Basic sanity checks
    assert((slab != NULL) && "Slab should be a valid pointer");

    debug("SLAB: allocation of size %i on cache %p\n", slab->size, slab);

#ifdef SLAB_CONFIG_DEBUG_FREELIST
    print_freelist(slab);
#endif // SLAB_CONFIG_DEBUG_FREELIST

#ifdef SLAB_CONFIG_DEBUG_PARANOID_ASSERTS
    debug("\t * Getting size at the start...");
    int start_size = get_freelist_size(slab);
    debug(" %d slots\n", start_size);
#endif // SLAB_CONFIG_DEBUG_PARANOID_ASSERTS

    struct slab_bufctl* freelist_array = (struct slab_bufctl*)(slab->freelist_buffer);
    int free_index = slab->freelist_start_index;
    debug("\t * Freelist start index is %i\n", free_index);

    // If the first node of the freelist is not free then all the buffers have been allocated.
    if(freelist_array[free_index].is_free != SLOT_FREE) {
        debug("\t * Can't allocate on this cache\n");
        return NULL;
    }

    // Increment the reference count
    slab->ref_count++;
    
    int free_next = freelist_array[free_index].next_index;
    int free_prev = freelist_array[free_index].prev_index;
    debug("\t * Index %i was found free\n", free_index);
    debug("\t * Index %i is new first\n", free_next);

    // Remove the node from the list. If it's the first one, then update the freelist_start_index
    // so that the freelist doesn't get destroyed.
    if(freelist_array[free_index].prev_index == NON_EXISTANT) {
        freelist_array[free_next].prev_index = NON_EXISTANT;
        slab->freelist_start_index = free_next;
    } else {
        freelist_array[free_prev].next_index = free_next;
        freelist_array[free_next].prev_index = free_prev;
    }

    // Append the node in the end of the list.
    freelist_array[slab->freelist_end_index].next_index = free_index;
    freelist_array[free_index].prev_index = slab->freelist_end_index;
    slab->freelist_end_index = free_index;
    freelist_array[free_index].next_index = NON_EXISTANT;

    // Important to mark it as busy.
    freelist_array[free_index].is_free = SLOT_BUSY;

    assert((slab->freelist_start_index != NON_EXISTANT));
    assert((slab->freelist_end_index != NON_EXISTANT));

#ifdef SLAB_CONFIG_DEBUG_PARANOID_ASSERTS
    debug("\t * Getting size at the start...");
    int after_size = get_freelist_size(slab);
    debug(" %d slots\n", start_size);
    assert((start_size == after_size) && "Freelist size changed :(");
#endif // SLAB_CONFIG_DEBUG_PARANOID_ASSERTS

#ifdef SLAB_CONFIG_DEBUG_FREELIST
    print_freelist(slab);
#endif // SLAB_CONFIG_DEBUG_FREELIST

    void* to_return = slab->allocable_buffer + slab->size * free_index;
    assert((is_ptr_in_page(slab, to_return)) && "Allocation pointer not inside the page cache");

    return to_return;
}

// TODO: fill with non allocated pattern 
// TODO: rewrite this pls.
void mem_slab_dealloc(struct mem_slab* slab, void* ptr) {
    // Basic sanity checks before beginning any work
    assert((slab != NULL) && "Slab should be a valid ptr");
    assert((is_ptr_in_page(slab, ptr)) && "Pointer was not allocated by this cache");

    struct slab_bufctl* freelist_array = (struct slab_bufctl*)(slab->freelist_buffer);
    uint16_t slot_index = get_buffer_index_from_ptr(slab, ptr);
    assert((slot_index != NON_EXISTANT));

    debug("SLAB: deallocating slot %i on cache %p\n", slot_index, slab);
    debug("\t * Freelist start is %d\n", slab->freelist_start_index);
    debug("\t * Freelist end is %d\n", slab->freelist_end_index);

#ifdef SLAB_CONFIG_DEBUG_PARANOID_ASSERTS
    debug("\t * Getting size at the start...");
    int start_size = get_freelist_size(slab);
    debug(" %d slots\n", start_size);
#endif // SLAB_CONFIG_DEBUG_PARANOID_ASSERTS


    // Check for double free
    assert((freelist_array[slot_index].is_free == SLOT_BUSY) && "Passed pointer has never been allocated or already free");
    
    // Decrement the reference count and mark node as free
    slab->ref_count--;
    freelist_array[slot_index].is_free = SLOT_FREE; 

    uint16_t next_index = freelist_array[slot_index].next_index;
    uint16_t prev_index = freelist_array[slot_index].prev_index;

    // The node is already the first in the list so no movement should be done.
    if(slot_index == slab->freelist_start_index) {
        return;
    }

    // Remove the node from the list
    if(next_index == NON_EXISTANT) {
        freelist_array[prev_index].next_index = NON_EXISTANT;
    } else {
        freelist_array[prev_index].next_index = next_index;
        freelist_array[next_index].prev_index = prev_index;
    }

    // Append the node to the start of the list and update the index to the first node
    freelist_array[slot_index].next_index = slab->freelist_start_index;
    freelist_array[slot_index].prev_index = NON_EXISTANT;
    freelist_array[slab->freelist_start_index].prev_index = slot_index;
    slab->freelist_start_index = slot_index;

    assert((slab->freelist_start_index != NON_EXISTANT));
    assert((slab->freelist_end_index != NON_EXISTANT));

    debug("\t * New first in the freelist %i\n", slab->freelist_start_index);
    debug("\t * New last in the freelist %i\n", slab->freelist_end_index);

#ifdef SLAB_CONFIG_DEBUG_PARANOID_ASSERTS
    debug("\t * Getting size at the start...");
    int after_size = get_freelist_size(slab);
    debug(" %d slots\n", start_size);
    assert((start_size == after_size) && "Freelist size changed :(");
#endif // SLAB_CONFIG_DEBUG_PARANOID_ASSERTS

#ifdef SLAB_CONFIG_DEBUG_FREELIST
    print_freelist(slab);
#endif // SLAB_CONFIG_DEBUG_FREELIST
}
