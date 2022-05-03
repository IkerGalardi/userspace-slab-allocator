#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

#include <valgrind/callgrind.h>

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
    printf("%i,", size);
    do_allocations_in_loop(BENCH_NUMBER_OF_ALLOCATIONS, size, mf, ff);
    printf("\n");
}

struct benchmark_settings {
    size_t allocation_size;
    malloc_function mf;
    free_function ff;
};

struct benchmark_settings get_settings_from_argv(int argc, char** argv) {
    struct benchmark_settings settings;

    bool has_functions = false;
    bool has_size = false;
    int option = 0;
    while((option = getopt(argc, argv, "t:s:")) != -1) {
        switch(option) {
            case 't':
                if(strcmp(optarg, "system") == 0) {
                    settings.mf = malloc;
                    settings.ff = free;
                } else if(strcmp(optarg, "slab") == 0) {
                    settings.mf = smalloc;
                    settings.ff = sfree;
                } else {
                    fprintf(stderr, "error: Unknown allocator type %s\n", optarg);
                    exit(1);
                }
                has_functions = true;
                break;

            case 's':
                settings.allocation_size = atoi(optarg);
                has_size = true;
                break;
        }
    }

    if(has_functions == false) {
        fprintf(stderr, "error: Forgot to specify what allocator to test (-t <system/slab>)\n");
    }

    if(has_size == false) {
        fprintf(stderr, "error: Forgot to specify size (-s <size>)\n");
    }
    
    if(has_functions == false || has_size == false) {
        exit(1);
    }

    return settings;
}

int main(int argc, char** argv) {
    struct benchmark_settings settings = get_settings_from_argv(argc, argv);

    smalloc_initialize();

    do_and_print_benchmark_for_size(settings.allocation_size, settings.mf, settings.ff);
}
