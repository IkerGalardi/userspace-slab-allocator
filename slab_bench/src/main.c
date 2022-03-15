#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <slab.h>
#include <smalloc.h>

#ifdef USE_SMALLOC
#define allocation_function smalloc
#define free_function       sfree
#else
#define allocation_function malloc
#define free_function       free
#endif // USE_SMALLOC
void do_allocations_in_loop(int count, int alloc_size) {
    void** allocations = malloc(count * sizeof(void*));

    // Do allocations
    printf("\t Allocating %i buffers... ");
    for(int i = 0; i < count; i++) {
        allocations[i] = allocation_function(alloc_size);
    }
    
    // Do allocations
    printf("\t Freeing %i buffers... ");
    for(int i = count - 1; i >= 0; i--) {
        free_function(allocations[i]);
    }
}

int main() {
    smalloc_initialize();
    
    printf("BENCH: 4 byte allocations:\n");
    do_allocations_in_loop(10000, 4);

    printf("BENCH: 16 byte allocations:\n");
    do_allocations_in_loop(10000, 16);

    printf("BENCH: 32 byte allocations:\n");
    do_allocations_in_loop(10000, 32);

    printf("BENCH: 128 byte allocations:\n");
    do_allocations_in_loop(10000, 128);

    printf("BENCH: 512 byte allocations:\n");
    do_allocations_in_loop(10000, 128);
}
