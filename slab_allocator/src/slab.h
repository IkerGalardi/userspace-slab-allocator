#ifndef _SLAB_H
#define _SLAB_H

#include <stdint.h>

// TODO: add multi threaded lock stuff
struct mem_slab {
    struct mem_slab* next;
    int size;
    int alignment;
    int ref_count;
    uint8_t data[];
};

struct slab_bufctl {
    struct slab_bufctl* next;
    uint8_t is_free;
};

// TODO: check if constructors and destructors are needed for userspace
struct mem_slab* mem_slab_create(int size, int alignment);
struct mem_slab* mem_slab_free(struct mem_slab* slab);

#endif // _SLAB_H
