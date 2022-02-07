#include "slab.h"

#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <errno.h>

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

    // Remove first from the list
    second->prev = NULL;
    slab->freelist_start = second;

    // Add it to as last element
    first->prev = last;
    first->next = NULL;
    last->next = first;
    slab->freelist_end = first;
}

static void move_node_to_start(struct mem_slab* slab, struct slab_bufctl* node) {
    struct slab_bufctl* previous = node->prev;
    struct slab_bufctl* next = node->next;

    // Remove node from the list
    previous->next = next;
    next->prev = previous;

    // Insert the node in the start of the list
    node->prev = NULL;
    node->next = slab->freelist_start;
    slab->freelist_start = node;
}

static int is_ptr_in_page(void* page, void* ptr, size_t ptr_size) {
    uint8_t* byte_page_ptr = (uint8_t*)page;
    uint8_t* byte_page_end_ptr = (uint8_t*)page + PAGE_SIZE;
    uint8_t* byte_ptr = (uint8_t*)ptr;
    uint8_t* byte_ptr_end = byte_ptr + ptr_size;

    return byte_ptr_end < byte_page_end_ptr;
}

struct mem_slab* mem_slab_create(int size, int alignment) {
    assert((size > 0) && "Slab size must be bigger than 0");

    // TODO: move the slab struct to the end of the buffer.
    // TODO: alignment requirements are ignored right now, dont do that :(
    // NOTE: important that MAP_PRIVATE or MAP_SHARED is added as flag.
    struct mem_slab* result = (struct mem_slab*)mmap(NULL, PAGE_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    fprintf(stderr, "Page size %i\n", PAGE_SIZE);
    fprintf(stderr, "SLAB: allocated page\n");
    fprintf(stderr, "      ptr   = %p\n", result);
    fprintf(stderr, "      size  = %i\n", size);
    fprintf(stderr, "      align = %i\n", alignment);
    
    // Linux returns -1 as address when no memory is mapped. If that happens return NULL and user should take care of that.
    if(result == (void*)-1) {
        perror("mmap");
        return NULL;
    }

    result->ref_count = 0;
    result->size = size;
    result->alignment = alignment;

    // Fill all the bufctl structures
    int i = 0;
    uint8_t* ptr = (uint8_t*)(result++);
    struct slab_bufctl* last = NULL;
    struct slab_bufctl* current = (struct slab_bufctl*)ptr;
    struct slab_bufctl* next = (struct slab_bufctl*)(ptr + sizeof(struct slab_bufctl) + size);
    while(is_ptr_in_page(result, current, sizeof(struct slab_bufctl) + size)) {
        current->prev = last;
        current->next = next;
        current->is_free = 0;

        // Advance the pointers
        ptr = (void*)next;
        last = current;
        current = next;
        next = (struct slab_bufctl*)(ptr + sizeof(struct slab_bufctl) + size);
        i++;
    }
    next->prev = current;
    next->next = NULL;
    next->is_free = 0;

    // Fill the free list members of the cache header.
    result->freelist_start = (struct slab_bufctl*)(result++);
    result->freelist_end = next;

    return result;
}

void mem_slab_free(struct mem_slab* slab) {
    assert((slab->ref_count == 0) && "Can't free the slab if objects are allocated");

    munmap((void*)slab, PAGE_SIZE);

    fprintf(stderr, "SLAB: freed page at %p\n", slab);
}

void* mem_slab_alloc(struct mem_slab* slab) {
    // TODO: cache expanding internally of by the user??
    // No empty buffer in the cache. Return NULL to notify the user.
    if(slab->freelist_start->is_free != 0) {
        fprintf(stderr, "SLAB: allocation failed because cache is full\n");
        return NULL;
    }

    struct slab_bufctl* bufctl = slab->freelist_start;
    bufctl->is_free = 1;

    move_first_node_to_end(slab);

    slab->ref_count++;

    return (void*)(bufctl++);
}

void  mem_slab_dealloc(struct mem_slab* slab, void* ptr) {
    uint8_t* byte_ptr = (uint8_t*)ptr;
    struct slab_bufctl* bufctl = (struct slab_bufctl*)(byte_ptr - sizeof(struct slab_bufctl));

    assert((bufctl->is_free != 0) && "Slot already free");
    bufctl->is_free = 0;

    // TODO: add scrub data to the buffer?

    move_node_to_start(slab, bufctl);

    slab->ref_count--;

    assert((slab->ref_count >= 0) && "Reference counting can't go bellow 0!");
}
