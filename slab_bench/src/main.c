#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <slab.h>
#include <smalloc.h>

#define BENCH_NUMBER_OF_ALLOCATIONS 10000

typedef void*(*malloc_function)(size_t);
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

static void do_and_print_benchmark_for_size(size_t size) {
    printf("\n%i,", size);
    do_allocations_in_loop(BENCH_NUMBER_OF_ALLOCATIONS, size, malloc, free);
    printf(",");
    do_allocations_in_loop(BENCH_NUMBER_OF_ALLOCATIONS, size, smalloc, sfree);
}

int main() {
    smalloc_initialize();

    printf("Alloc size,System malloc,System free,Slabbed malloc,Slabbed free");    
    do_and_print_benchmark_for_size(4);   
    do_and_print_benchmark_for_size(16);   
    do_and_print_benchmark_for_size(32);   
    do_and_print_benchmark_for_size(128);   
    do_and_print_benchmark_for_size(512);   
    printf("\n");
}
