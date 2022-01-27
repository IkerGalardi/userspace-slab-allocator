#ifndef _SLAB_H
#define _SLAB_H

// TODO: add multi threaded lock stuff
struct mem_slab {
    int size;
    int alignment;
};

// TODO: check if constructors and destructors are needed for userspace
struct mem_slab* mem_slab_create(int size, int alignment);

#endif // _SLAB_H
