#include "cache_pool.h"

/*
 * Returns the pointer to the start of the page given a pointer.
 *
 * @param ptr: pointer to find the start of the page
 * @return: pointer to the start of the page of 'ptr'
 */
static void* get_page_pointer(void* ptr) {
    // NOTE: assumes 4k pages. maybe some way to detect 16k pages?
    return (void*)((uintptr_t)ptr & (~0xFFF));
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

struct slab_pool slab_pool_create(size_t allocation_size) {
    struct mem_slab* first_slab = mem_slab_create(allocation_size, 0);

    struct slab_pool result;
    result.list_start = first_slab;
    result.list_end = first_slab;
    result.allocation_size = allocation_size;

    return result;
}

/*
 * Returns a non-full slab from a pool. If there is no slab cache available it will allocate a new 
 * one and return it. If a NULL slab is returned the system is probably OOM.
 */
static struct mem_slab* get_slab_with_enough_space(struct slab_pool pool) {
    struct mem_slab* first_slab = pool.list_start;

    // If the first slab has enough space simply return the first slab.
    if(first_slab->ref_count < first_slab->max_refs) {
        return first_slab;
    }

    // Create a new slab and append it to the start of the pool
    struct mem_slab* new_first = mem_slab_create(pool.allocation_size, 0);
    first_slab->prev = new_first;
    new_first->next = first_slab;
    pool.list_start = new_first;

    return new_first;
}

void* slab_pool_allocate(struct slab_pool pool) {
    struct mem_slab* slab_with_space = get_slab_with_enough_space(pool);
    
    // Allocate the pointer to be returned
    void* result = mem_slab_alloc(slab_with_space);

    // Check if the slab is full, if it is move it to the end of the caches to ensure that
    // caches with enough capacity to allocate will always be at the start.
    bool slab_full = slab_with_space->ref_count == slab_with_space->max_refs;
    if(slab_full && (slab_with_space != pool.list_end)) {
        struct mem_slab* previous = slab_with_space->prev;
        struct mem_slab* next =     slab_with_space->next;

        previous->next = next;
        next->prev = previous;
        slab_with_space->prev = pool.list_end;
        pool.list_end->next = slab_with_space;
        pool.list_end = slab_with_space;
    }

    return result;
}

bool slab_pool_deallocate(struct slab_pool pool, void* ptr) {
    struct mem_slab* slab = is_ptr_allocated_in_pool(pool.list_start, ptr);
    
    // If the pointer was not allocated on any slab then simply return false.
    if(slab == NULL)
        return false;

    // If the slab is not already at the start of the pool, move it to the start.
    if(slab != pool.list_start) {
        struct mem_slab* list_start = pool.list_start;
        struct mem_slab* previous = slab->prev;
        struct mem_slab* next = slab->next;

        previous->next = next;
        next->prev = previous;
        list_start->prev = slab;
        slab->prev = NULL;
        slab->next = list_start;
        pool.list_start = slab;
    }

    mem_slab_dealloc(pool.list_start, ptr);
    return true;
}
