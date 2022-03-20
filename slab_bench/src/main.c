#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <slab.h>
#include <smalloc.h>

typedef void*(*malloc_function)(int);
typedef void(*free_function)(void*);

int64_t difftimespec_ns(const struct timespec after, const struct timespec before)
{
    return ((int64_t)after.tv_sec - (int64_t)before.tv_sec) * (int64_t)1000000000
         + ((int64_t)after.tv_nsec - (int64_t)before.tv_nsec);
}

void do_allocations_in_loop(int count, int alloc_size, malloc_function mf, free_function ff) {
    void** allocations = malloc(count * sizeof(void*));

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Do allocations
    for(int i = 0; i < count; i++) {
        allocations[i] = mf(alloc_size);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    int64_t nanosecs = difftimespec_ns(end, start);
    printf("%i,", nanosecs);

    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Do allocations
    for(int i = count - 1; i >= 0; i--) {
        ff(allocations[i]);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    nanosecs = difftimespec_ns(end, start);
    printf("%i", nanosecs);
}

int main() {
    smalloc_initialize();

    printf(",System malloc,System free,Slabbed malloc,Slabbed free\n");    
    
    // Size 4 allocations
    printf("4,");
    do_allocations_in_loop(10000, 4, malloc, free);
    printf(",");
    do_allocations_in_loop(10000, 4, smalloc, sfree);

    printf("\n16,");
}
