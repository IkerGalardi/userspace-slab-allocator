#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

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

static void do_and_print_benchmark_for_size(size_t size, malloc_function mf, free_function ff) {
    printf("\n%i,", size);
    do_allocations_in_loop(BENCH_NUMBER_OF_ALLOCATIONS, size, mf, ff);
}

int main(int argc, char** argv) {
    if(argc != 2) {
        fprintf(stderr, "Wrong usage: ./bench <system|slab>\n");
        exit(1);
    }

    smalloc_initialize();

    printf("Alloc size,Allocation times,Deallocation times");    
    if(strcmp(argv[1], "system") == 0) {
        do_and_print_benchmark_for_size(4, malloc, free);   
        do_and_print_benchmark_for_size(16, malloc, free);
        do_and_print_benchmark_for_size(32, malloc, free);
        do_and_print_benchmark_for_size(128, malloc, free);   
        do_and_print_benchmark_for_size(512, malloc, free);   
    } else if(strcmp(argv[1], "slab") == 0){
        do_and_print_benchmark_for_size(4, smalloc, sfree);   
        do_and_print_benchmark_for_size(16, smalloc, sfree);
        do_and_print_benchmark_for_size(32, smalloc, sfree);
        do_and_print_benchmark_for_size(128, smalloc, sfree);   
        do_and_print_benchmark_for_size(512, smalloc, sfree);   
    }
    printf("\n");
}
