#include "slab_pool.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

#include "internal_assert.h"
#include "utils.h"

#ifdef POOL_CONFIG_DEBUG
    #define debug(...) fprintf(stderr, __VA_ARGS__); fflush(stderr)
#else
    #define debug(...)
#endif

#define POOL_START_SIZE  10 
#define POOL_GROW_RATE   5 
#define POOL_MAX_GROW_RATE   5 


#define POOL_PAGE_SIZE sysconf(_SC_PAGESIZE)

int pool_stat_grow_count = 0;


static struct mem_slab* get_last_from_list(struct mem_slab* slab) {
    struct mem_slab* current = slab;
    while(current->next != NULL) {
        current = current->next;
    }
    return current;
}

/*
 * Finds if a pointer is allocated in a pool of slabs.
 *
 * @param pool: pointer to the first element of slab pool
 * @param ptr: pointer to find in pool
 *
 * @return: the slab where 'ptr' is allocated, NULL if 'ptr' was not allocated in the pool.
 */
static struct mem_slab* is_ptr_allocated_in_pool(struct mem_slab* list_start, void* ptr) {
    struct mem_slab* current_slab = list_start;
    while(current_slab != NULL) {
        // If the pointer is in the same page as the slab, then the pointer
        // was allocated on that slab
        if(current_slab == get_page_pointer(ptr)) {
            return current_slab;
        }

        current_slab = current_slab->next;
    }

    return NULL;
}

/*
 * Returns the size of the slab list. Only for debugging purposes.
 */
static int get_list_size(struct mem_slab* list_start) {
    struct mem_slab* current = list_start;
    int jumps = 0;

    while(current != NULL) {
        fflush(stdout);
        jumps++;
        current = current->next;
    }

    return jumps;
}

/*
 * Moves a slab to the end of the pool, updating the list_start and list_end if necessary.
 */
static void move_slab_to_end_of_the_list(struct slab_pool* pool, struct mem_slab* slab) {
    struct mem_slab* previous = slab->prev;
    struct mem_slab* next =     slab->next;

    // If the slab is already in the end, we don't need to do anything
    if(next ==  NULL) {
        return;
    }

    // Remove the node from the list. If the node is the first, then update the pointer to
    // the first node.
    if(previous == NULL) {
        next->prev = NULL;
        pool->list_start = next;
    } else {
        previous->next = next;
        next->prev = previous;
    }

    // Append the node to the end of the list
    pool->list_end->next = slab;
    slab->prev = pool->list_end;
    pool->list_end = slab;
    slab->next = NULL;
}

static void move_slab_to_start_of_the_list(struct slab_pool* pool, struct mem_slab* slab) {
    struct mem_slab* list_start = pool->list_start;
    struct mem_slab* previous = slab->prev;
    struct mem_slab* next = slab->next;

    if(pool->list_start == slab) {
        return;
    }

    // Remove node from the list
    if(next == NULL) {
        previous->next = NULL;
        pool->list_end = slab->prev;
    } else {
        previous->next = next;
        next->prev = previous;
    }

    // Insert the node at the start of the list
    slab->next = list_start;
    slab->prev = NULL;
    list_start->prev = slab;
    pool->list_start = slab;
}

struct slab_pool slab_pool_create(size_t allocation_size) {
    struct mem_slab* first_slab = mem_slab_create_several(allocation_size, 0, POOL_START_SIZE, NULL);

    struct slab_pool result;
    result.list_start = first_slab;
    result.list_end = get_last_from_list(first_slab);
    result.allocation_size = allocation_size;

    result.allocation_count = 0;
    result.deallocation_count = 0;

#ifdef POOL_CONFIG_DEBUG
    debug("POOL: created pool of size %i\n", result.allocation_size);
    int size = get_list_size(result.list_start);
    debug("\t* Size of the list at the start is %i\n", size);
#endif // POOL_CONFIG_DEBUG

    return result;
}

/*
 * Returns a non-full slab from a pool. If there is no slab cache available it will allocate a new
 * one and return it. If a NULL slab is returned the system is probably OOM.
 */
static struct mem_slab* get_slab_with_enough_space(struct slab_pool* pool) {
    struct mem_slab* first_slab = pool->list_start;
    debug("\t* First slab of the list is %p\n", first_slab);

    assert((first_slab != NULL) && "List broken");

    size_t grow_rate_multiplier = 1;
    if(pool->allocation_count > pool->deallocation_count && pool->deallocation_count != 0) {
        size_t difference = pool->allocation_count - pool->deallocation_count;
        grow_rate_multiplier = ((difference < POOL_MAX_GROW_RATE) ? (difference) : POOL_MAX_GROW_RATE);
    }

#ifdef POOL_CONFIG_DEBUG
    int size = get_list_size(pool->list_start);
    debug("\t* Size of the list at the start is %i\n", size);
#endif // POOL_CONFIG_DEBUG

    // If the first slab has enough space simply return the first slab.
    if(first_slab->ref_count < first_slab->max_refs) {
        debug("\t\t * First slab already free, returning %p\n", first_slab);
        debug("\t\t * Reference count is %i\n", first_slab->ref_count);
        debug("\t\t * Max allocations are %i\n", first_slab->max_refs);
        return first_slab;
    }

    debug("\t\t * First slab not free, need to grow the slab list\n");
#ifdef POOL_CONFIG_PARANOID_ASSERTS
    int list_size_before_growing = get_list_size(pool->list_start);
    debug("\t\t * Finished getting the list size: %i\n", list_size_before_growing);
#endif

    // Check the next. This is done because when creating several caches at the same
    // time we have several free slots.
    move_slab_to_end_of_the_list(pool, first_slab);
    first_slab = pool->list_start;
    if(first_slab->ref_count < first_slab->max_refs) {
        debug("\t\t * First slab already free, returning %p\n", first_slab);
        debug("\t\t * Reference count is %i\n", first_slab->ref_count);
        debug("\t\t * Max allocations are %i\n", first_slab->max_refs);
        return first_slab;
    }

    // Create a new slab and append it to the start of the pool
    struct mem_slab* new_first = mem_slab_create_several(pool->allocation_size, 
                                                         0, 
                                                         POOL_GROW_RATE * grow_rate_multiplier, 
                                                         first_slab);
    pool->list_start = new_first;
    debug("\t\t * Appended new slab %p to the list\n", new_first);

    pool_stat_grow_count++;

#ifdef POOL_CONFIG_PARANOID_ASSERTS
    int list_size_after_growing = get_list_size(pool->list_start);
    debug("\t\t * Finished getting the list size: %i\n", list_size_before_growing);
    //assert((list_size_before_growing == list_size_after_growing - POOL_GROW_RATE));

    assert((first_slab != pool->list_start));
#endif

    return new_first;
}

void* slab_pool_allocate(struct slab_pool* pool) {
    debug("POOL: allocating memory...\n");
    debug("\t* Getting free slab...\n");
    struct mem_slab* slab_with_space = get_slab_with_enough_space(pool);
    assert((slab_with_space != NULL) && "NULL slab returned from get_slab_with_enough_space");

    pool->allocation_count++;

    // Check if the slab is full, if it is move it to the end of the caches to ensure that
    // caches with enough capacity to allocate will always be at the start.
    bool slab_full = slab_with_space->ref_count == slab_with_space->max_refs;
    debug("\t* Slab full? %i\n", slab_full); 
    if(slab_full && (slab_with_space != pool->list_end)) {
        debug("\t* Moving the slab to the end of the cache\n"); 

        move_slab_to_end_of_the_list(pool, slab_with_space);
    }

    // Allocate the pointer to be returned
    void* result = mem_slab_alloc(slab_with_space);

#ifdef POOL_CONFIG_PARANOID_ASSERTS
    assert((result != NULL) && "Slab full when it should not");
#endif

    debug("\t* Returning\n"); 
    return result;
}

bool slab_pool_deallocate(struct slab_pool* pool, void* ptr) {
    debug("POOL: deallocating pointer %p\n", ptr);

    pool->deallocation_count++;

    // Fast path. If there is no magic number or the size is not the same, then we simply return that the pointer
    // was not allocater on this pool.
    struct mem_slab* slab = get_page_pointer(ptr);
    if(slab->slab_magic != SLAB_MAGIC_NUMBER || slab->size != pool->allocation_size) {
        return false;
    }

    debug("\t* Slab containing pointer is %p\n", slab);
    
    // If the pointer was not allocated on any slab then simply return false.
    if(slab == NULL) {
        debug("\t* Slab not on this pool, returning\n");
        return false;
    }

    // If the slab is not already at the start of the pool, move it to the start.
    if(slab != pool->list_start) {
        debug("\t* Freed slab is not first in the list, moving it\n");

        move_slab_to_start_of_the_list(pool, slab);
    }

    debug("\t* Freeing in the slab\n");
    mem_slab_dealloc(pool->list_start, ptr);
    
    // TODO: investigate if madvise(MADVISE_DONTUSE) is better in terms of performance
    //       and how to implement something like that.
    if((pool->list_start->ref_count == 0) && 
       (pool->list_start != pool->list_end) &&
       (pool->allocation_count < pool->deallocation_count)) {
        struct mem_slab* to_delete = pool->list_start;
        struct mem_slab* previous = slab->prev;
        struct mem_slab* next =     slab->next;

        pool->list_start = next;
        next->prev = NULL;

        mem_slab_free(to_delete);
    }

    return true;
}
