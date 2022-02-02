#include "slab.h"

#include <stddef.h>
#include <assert.h>

#include <sys/mman.h>

#define PAGE_SIZE 4 * 1024

/*
 * Memory layout of cache page
 * +-----------+--------+--------+--------+--------+ 
 * |   HEADER  | BUFCTL |  DATA  | BUFCTL |  DATA  | ....
 * +-----------+--------+--------+--------+--------+ 
 *
 */

static void move_first_node_to_end(struct mem_slab* slab) {
    struct slab_bufctl* first = slab->freelist_start;
    struct slab_bufctl* second = first->next;
    struct slab_bufctl* last = slab->freelist_end;

    second->prev = NULL;
    slab->freelist_start = second;

    first->prev = last;
    first->next = NULL;
    last->next = first;
    slab->freelist_end = first;
}

static void move_node_to_start(struct mem_slab* slab, struct slab_bufctl* node) {

}

struct mem_slab* mem_slab_create(int size, int alignment) {
    assert((size > 0) && "Slab size must be bigger than 0");

    // TODO: move the slab struct to the end of the buffer.
    // TODO: alignment requirements are ignored right now, dont do that :(
    struct mem_slab* result = mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);

    // If we can't get memory from the kernel we just return NULL to notify the user
    // something went wrong.
    if(result == NULL)
        return NULL;

    result->size = size;
    result->alignment = alignment;

    const int num_slots = (PAGE_SIZE - sizeof(struct mem_slab)) / (sizeof(struct slab_bufctl) + size);

    // Fill all the bufctl structures as explained on the memory layout
    uint8_t* ptr = (uint8_t*)(result++);
    for(int i = 0; i < num_slots - 1; i++) {
        struct slab_bufctl* bufctl_ptr = (struct slab_bufctl*)ptr;
        uint8_t* next_ptr = ptr + sizeof(struct slab_bufctl) + size;

        bufctl_ptr->is_free = 0;
        bufctl_ptr->next = (struct slab_bufctl*)next_ptr;
        bufctl_ptr->prev = (struct slab_bufctl*)(ptr - sizeof(struct slab_bufctl) - size);

        ptr = next_ptr;
    }

    // Last bufctl's next pointer needs to be NULL to indicate the end of the cache page.
    struct slab_bufctl* bufctl_ptr = (struct slab_bufctl*)ptr;
    bufctl_ptr->is_free = 0;
    bufctl_ptr->next = NULL;

    // Fill the free list members of the cache header.
    result->freelist_start = (struct slab_bufctl*)(result++);
    result->freelist_end = bufctl_ptr;

    return result;
}

void mem_slab_free(struct mem_slab* slab) {
    // TODO: assert for slab is free?
    assert((slab->ref_count > 0) && "Use after free of slab");

    // Decrement the reference count and if no longer referenced free 
    // the memory.
    slab->ref_count--;
    if(slab->ref_count == 0) {
        munmap((void*)slab, PAGE_SIZE);
    }
}

void* mem_slab_alloc(struct mem_slab* slab) {
    // TODO: cache expanding internally of by the user??
    // No empty buffer in the cache. Return NULL to notify the user.
    if(slab->freelist_start->is_free != 0) {
        return NULL;
    }

    struct slab_bufctl* bufctl = slab->freelist_start;
    bufctl->is_free = 1;

    move_first_node_to_end(slab);

    return (void*)(bufctl++);
}

void  mem_slab_dealloc(struct mem_slab* slab, void* ptr) {
    uint8_t* byte_ptr = (uint8_t*)ptr;
    struct slab_bufctl* bufctl = (struct slab_bufctl*)(byte_ptr - sizeof(struct slab_bufctl));

    assert((bufctl->is_free != 0) && "Slot already free");
    bufctl->is_free = 0;

    // TODO: add scrub data to the buffer?

    move_node_to_start(slab, bufctl);
}
