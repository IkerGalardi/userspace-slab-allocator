#include "slab_pool.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

//#define DEBUG_ASSERTS
#include "internal_assert.h"
#include "utils.h"

#ifdef POOL_CONFIG_DEBUG
    #define debug(...) fprintf(stderr, __VA_ARGS__); fflush(stderr)
#else
    #define debug(...)
#endif

/*
 * Returns the size of the slab list. Only for debugging purposes.
 */
MAYBE_UNUSED static size_t get_list_size(struct mem_slab* list_start) {
    struct mem_slab* current = list_start;
    int jumps = 0;

    while(current != NULL) {
        fflush(stdout);
        jumps++;
        current = current->next;
    }

    return jumps;
}

static struct mem_slab* get_last_from_list(struct mem_slab* slab) {
    struct mem_slab* current = slab;
    while(current->next != NULL) {
        current = current->next;
    }
    return current;
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
    const size_t pool_start_size = 20;
    struct mem_slab* first_slab = mem_slab_create_several(allocation_size, 0, pool_start_size, NULL);
    assert(get_list_size(first_slab) == pool_start_size);

    struct slab_pool result;
    result.list_start = first_slab;
    result.list_end = get_last_from_list(first_slab);
    result.allocation_size = allocation_size;

    result.params.minimum_empty_slabs = 30;
    result.params.default_grow_rate = 10;
    result.params.max_grow_rate = 30;
    
    result.data.allocation_count = 0;
    result.data.deallocation_count = 0;
    result.data.total_count = 40;
    result.data.full_count = 0;

    return result;
}

void* slab_pool_allocate(struct slab_pool* pool) {
    // TODO: add paranoid asserts with slab size.
    assert(pool != NULL);
    assert(pool->list_start != NULL);
    
    pool->data.allocation_count++;
    
    // If the first slab has space, simply allocate on it and return.
    bool is_first_full = pool->list_start->ref_count == pool->list_start->max_refs;
    if(!is_first_full) {
        void* result = mem_slab_alloc(pool->list_start);
        assert(result != NULL);
        
        return result;
    }
                                                     
    // If the first slab has no space, we need to move it to the end of the list
    // and check the next one as creating several can lead to a situation where the 
    // first is full but the next are completelly empty.
    move_slab_to_end_of_the_list(pool, pool->list_start);
    is_first_full = pool->list_start->ref_count == pool->list_start->max_refs;
    if(!is_first_full) {
        void* result = mem_slab_alloc(pool->list_start);
        assert(result != NULL);
        
        return result;
    } 
    
    // If the second one is still full, we have no choice but to create more 
    // slabs. Important to move the full slab to the end in order to maintain 
    // favorable ordering.
    move_slab_to_end_of_the_list(pool, pool->list_start);
    size_t grow_count = heuristic_decision_grow_count(pool->params, pool->data);
    struct mem_slab* new_first = mem_slab_create_several(pool->allocation_size,
                                                         0,
                                                         grow_count,
                                                         pool->list_start);
    pool->list_start = new_first;
    
    pool->data.total_count += grow_count;
    assert(pool->data.total_count == get_list_size(pool->list_start));
    
    // Allocate in the newly created slab and return.
    void* result = mem_slab_alloc(pool->list_start);
    assert(result != NULL);
    return result;
}

void slab_pool_deallocate(struct slab_pool* pool, void* ptr) {
    assert(pool != NULL);
    assert(ptr != NULL);
    
    pool->data.deallocation_count++;
    
    struct mem_slab* slab = (struct mem_slab*)get_page_pointer(ptr);
    bool was_slab_full = slab->ref_count == slab->max_refs;
    
    mem_slab_dealloc(slab, ptr);
    
    
    // If the slab was full before deallocating the pointer we need to move it
    // to the start of the list. This way, we can maintain free slabs first in order
    // to speedup allocations. No need to check if the slab needs to be freed as
    // its imposible it has 0 allocations in it.
    if(was_slab_full) { 
        pool->data.full_count--;
        if(slab != pool->list_start) {
            move_slab_to_start_of_the_list(pool, slab);
            return;
        }
    }
                                                                                            
    // Ask hour super good heuristic if we need to nuke the slab from the list.
    bool is_empty = slab->ref_count == 0;
    bool is_lonely = slab->next == NULL && slab->prev == NULL;
    if(is_empty && !is_lonely) {
        bool heuristic_decision = heuristic_decision_does_unmap(pool->params, pool->data);
        if(heuristic_decision) {
            //printf("DELETING\n");
            struct mem_slab* prev = slab->prev;
            struct mem_slab* next = slab->next;
            if(prev == NULL) {
                next->prev = NULL;
                pool->list_start = next;
            } else {
                prev->next = next;
                next->prev = prev;
            }
            mem_slab_free(slab);
            pool->data.total_count--;
        }
        
        assert(pool->data.total_count == get_list_size(pool->list_start));
    }
}
