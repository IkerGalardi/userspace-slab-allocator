#include "slab.h"

#include <sys/mman.h>
#include <stddef.h>

struct mem_slab* mem_slab_create(int size, int alignment) {
    // TODO: move the slab struct to the end of the buffer.
    // TODO: alignment requirements are ignored right now, dont do that :(
    struct mem_slab* result = mmap(NULL, 1024 * 4, PROT_READ | PROT_WRITE, MAP_ANON, -1, 0);
    result->size = size;
    result->alignment = alignment;



    return result;
}
