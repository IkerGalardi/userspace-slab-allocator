#include "slab.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include <sys/mman.h>

#include "internal_assert.h"
#include "utils.h"

#define SLAB_PAGE_SIZE sysconf(_SC_PAGESIZE)


// State in which a slot inside a slab can be.
#define SLOT_FREE 0
#define SLOT_BUSY 1

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
    // If the pointer is calculated with the next formula, the index should be
    // calculated like the return statement.
    //      ptr = allocable_buffer + slab->size * index;
    //                        |
    //                        v
    //      index = (ptr - allocable_buffer) / (slab->size)

    return ((uintptr_t)ptr - (uintptr_t)slab->allocable_buffer) / (slab->size);
}

/*
 * Returns the size of the slab list. Should only be used when debugging is enabled
 * as traversing the slows down things.
 */
MAYBE_UNUSED static size_t get_freelist_size(struct mem_slab* slab) {
    int count = 0;

    int current_index = slab->freelist_start_index;
    struct slab_bufctl* bufctl_array = (struct slab_bufctl*)(slab->freelist_buffer);
    while(bufctl_array[current_index].next_index != NON_EXISTANT) {
        current_index = bufctl_array[current_index].next_index;
        count++;
    }
    
    return count;
}

/*
 * Prepares the page to be used as a slab by filling the header structure.
 */
static void prepare_slab_header(struct mem_slab* result, int size, int alignment) {
    result->slab_magic = SLAB_MAGIC_NUMBER;
    result->ref_count = 0;
    result->size = size;
    result->alignment = alignment;
    result->freelist_buffer = (struct slab_bufctl*)(result + 1);
    result->next = NULL;
    result->prev = NULL;

    // Taking into account the next relation, we can calculate the number of buffers
    // that can be saved on a page:
    //          sizeof(mem_slab) + num_buffers * (buff_size * sizeof(bufctl)) = SLAB_PAGE_SIZE
    int num_buffers = (SLAB_PAGE_SIZE - sizeof(struct mem_slab))/(size * sizeof(struct slab_bufctl));


    // Link all the free list
    // TODO: maybe can avoid linking all the freelist at the start and do it
    //       with each allocation. Investigate that.
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

    // TODO: take into account alignment pls
    // TODO: non allocated pattern or something should be added
    result->allocable_buffer = (void*)((uintptr_t)result->freelist_buffer + num_buffers);
}

struct mem_slab* mem_slab_create(int size, int alignment) {
    assert(size > 0);
    assert(alignment >= 0);

    struct mem_slab* result = (struct mem_slab*)mmap(NULL,
                                                     SLAB_PAGE_SIZE,
                                                     PROT_READ | PROT_WRITE,
                                                     MAP_ANONYMOUS | MAP_PRIVATE,
                                                     -1,
                                                     0);
    
    // Linux returns -1 as address when no memory is mapped. If that happens
    // return NULL and user should take care of that.
    if(result == (void*)-1) {
        return NULL;
    }

    prepare_slab_header(result, size, alignment);

    return result;
}

struct mem_slab* mem_slab_create_several(int size, int alignment, int count, struct mem_slab* list_next) {
    size_t mapping_size = SLAB_PAGE_SIZE * count;
    void* mapped_region = mmap(NULL, 
                               mapping_size, 
                               PROT_READ | PROT_WRITE, 
                               MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, 
                               -1, 
                               0);
    uint8_t* result_in_bytes = (uint8_t*)mapped_region;

    // Linux returns -1 as address when no memory is mapped. If that happens
    // return NULL and user should take care of that.
    if(result_in_bytes == (void*)-1) {
        return NULL;
    }

    // Prepare all the mapped pages
    struct mem_slab* first = (struct mem_slab*)mapped_region;
    prepare_slab_header(first, size, alignment);
    first->prev = NULL;
    first->next = (struct mem_slab*)(result_in_bytes + SLAB_PAGE_SIZE);
    for(int i = 1; i < count; i++) {
        struct mem_slab* previous = (struct mem_slab*)((i-1) * SLAB_PAGE_SIZE + result_in_bytes);
        struct mem_slab* current =  (struct mem_slab*)((i+0) * SLAB_PAGE_SIZE + result_in_bytes);
        struct mem_slab* next =     (struct mem_slab*)((i+1) * SLAB_PAGE_SIZE + result_in_bytes);

        // Prepare the header and link the slabs
        prepare_slab_header(current, size, alignment);
        current->prev = previous;
        current->next = next;
    }
    struct mem_slab* last =  (struct mem_slab*)((count-1) * SLAB_PAGE_SIZE + result_in_bytes);
    last->next = list_next;

    // If there is a next, link that next to the already created list
    if(list_next != NULL) {
        list_next->prev = last;
    }

    return first;
}

void mem_slab_free(struct mem_slab* slab) {
    assert(slab != NULL);
    assert(slab->slab_magic == SLAB_MAGIC_NUMBER);
    assert(slab->ref_count == 0);

    munmap((void*)slab, SLAB_PAGE_SIZE);
}

void* mem_slab_alloc(struct mem_slab* slab) {
    // Basic sanity checks
    assert(slab != NULL);
    assert(slab->slab_magic == SLAB_MAGIC_NUMBER);

#ifdef SLAB_CONFIG_DEBUG_PARANOID_ASSERTS
    int start_size = get_freelist_size(slab);
#endif // SLAB_CONFIG_DEBUG_PARANOID_ASSERTS

    struct slab_bufctl* freelist_array = (struct slab_bufctl*)(slab->freelist_buffer);
    int free_index = slab->freelist_start_index;

    // If the first node of the freelist is not free then all the buffers have
    // been allocated.
    if(freelist_array[free_index].is_free != SLOT_FREE) {
        return NULL;
    }

    // Increment the reference count
    slab->ref_count++;
    
    int free_next = freelist_array[free_index].next_index;
    int free_prev = freelist_array[free_index].prev_index;

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

    assert(slab->freelist_start_index != NON_EXISTANT);
    assert(slab->freelist_end_index != NON_EXISTANT);

#ifdef SLAB_CONFIG_DEBUG_PARANOID_ASSERTS
    int after_size = get_freelist_size(slab);
    assert((start_size == after_size) && "Freelist size changed :(");
#endif // SLAB_CONFIG_DEBUG_PARANOID_ASSERTS

    void* to_return = (void*)((uintptr_t)slab->allocable_buffer + slab->size * free_index);
    assert(is_ptr_in_page(slab, to_return));

    return to_return;
}

// TODO: fill with non allocated pattern 
void mem_slab_dealloc(struct mem_slab* slab, void* ptr) {
    // Basic sanity checks before beginning any work
    assert(slab != NULL);
    assert(slab->slab_magic == SLAB_MAGIC_NUMBER);
    assert(is_ptr_in_page(slab, ptr));

    struct slab_bufctl* freelist_array = (struct slab_bufctl*)(slab->freelist_buffer);
    uint16_t slot_index = get_buffer_index_from_ptr(slab, ptr);
    assert(slot_index != NON_EXISTANT);

    // Check for double free
    assert(freelist_array[slot_index].is_free == SLOT_BUSY);
    
    // Decrement the reference count and mark node as free
    slab->ref_count--;
    freelist_array[slot_index].is_free = SLOT_FREE; 
    
    uint16_t next_index = freelist_array[slot_index].next_index;
    uint16_t prev_index = freelist_array[slot_index].prev_index;

    // The node is already the first in the list so no movement should be done.
    if(slot_index == slab->freelist_start_index) {
        return;
    }

    if(!freelist_array[prev_index].is_free) {
        return;
    }

    // Remove the node from the list
    if(next_index == NON_EXISTANT) {
        freelist_array[prev_index].next_index = NON_EXISTANT;
    } else {
        freelist_array[prev_index].next_index = next_index;
        freelist_array[next_index].prev_index = prev_index;
    }

    // If the slot is at the end we need to move the end index to the before one.
    if(slab->freelist_end_index == slot_index) {
        slab->freelist_end_index = prev_index;
    }

    // Append the node to the start of the list and update the index to the first node
    freelist_array[slot_index].next_index = slab->freelist_start_index;
    freelist_array[slot_index].prev_index = NON_EXISTANT;
    freelist_array[slab->freelist_start_index].prev_index = slot_index;
    slab->freelist_start_index = slot_index;
    
    assert(slab->freelist_start_index != NON_EXISTANT);
    assert(slab->freelist_end_index != NON_EXISTANT);
}
