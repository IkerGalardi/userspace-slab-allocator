#include <stdio.h>

#include <slab.h>

int main(int argc, char** argv) {
    printf("Hello world!\n");

    struct mem_slab* slab = mem_slab_create(sizeof(int), 0);

    void* allocated[5];
    for(int i = 0; i < 5; i++) {
        allocated[i] = mem_slab_alloc(slab);
    }

    mem_slab_dealloc(slab, allocated[2]);

    void* a = mem_slab_alloc(slab);
}
