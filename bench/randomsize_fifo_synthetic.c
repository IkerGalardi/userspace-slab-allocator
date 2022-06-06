#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <getopt.h>

#include <smalloc.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#ifdef USE_SMALLOC
    #define allocate(x)   malloc(x)
    #define deallocate(x) free(x)
#else
    #define allocate(x)   malloc(x)
    #define deallocate(x) free(x)
#endif // USE_SMALLOC

#define ITERATION_COUNT 100
#define ALLOCATION_COUNT 1000000

#define MIN_ALLOCATION_SIZE 1
#define MAX_ALLOCATION_SIZE 800

#define ALPHA 2
#define BETA  20

uint64_t us_spent_allocating = 0;
uint64_t us_spent_deallocating = 0;

static inline uint64_t timespec_diff_ns(struct timespec *start, struct timespec *end)
{
    struct timespec result;
    if ((end->tv_nsec - start->tv_nsec) < 0) {
        result.tv_sec = end->tv_sec - start->tv_sec - 1;
        result.tv_nsec = end->tv_nsec - start->tv_nsec + 1000000000L;
    } else {
        result.tv_sec = end->tv_sec - start->tv_sec;
        result.tv_nsec = end->tv_nsec - start->tv_nsec;
    }

    return (result.tv_sec * 1000000000L) + result.tv_nsec;
}

struct benchmark_parameters {
    uint32_t alpha;
    uint32_t beta;
};

struct benchmark_parameters get_params(int argc, char** argv) {
    struct benchmark_parameters result = {
        .alpha = 2,
        .beta = 20
    };

    int option = 0;
    while((option = getopt(argc, argv, "a:b:")) != -1) {
        switch(option) {
        case 'a':
            result.alpha = atoi(optarg);
            break;

        case 'b':
            result.beta = atoi(optarg);
            break;
        }
    }

    return result;
}

int main(int argc, char** argv) {
    smalloc_initialize();

    struct benchmark_parameters params = get_params(argc, argv);

    gsl_rng* r = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(r, 0);

    void** allocation = malloc(ALLOCATION_COUNT * sizeof(void*));

    uint64_t time_allocating_ns = 0;
    uint64_t time_deallocating_ns = 0;
    for(int it = 0; it < ITERATION_COUNT; it++) {
        struct timespec start;
        struct timespec end;

        for(int i = 0; i < ALLOCATION_COUNT; i++) {
            int allocation_size = roundf(MAX_ALLOCATION_SIZE *
                                         gsl_ran_beta(r, params.alpha, params.beta));

            clock_gettime(CLOCK_MONOTONIC, &start);
            allocation[i] = allocate(allocation_size); 
            clock_gettime(CLOCK_MONOTONIC, &end);

            time_allocating_ns += timespec_diff_ns(&start, &end);
        }


        for(int i = 0; i < ALLOCATION_COUNT; i++) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            deallocate(allocation[i]); 
            clock_gettime(CLOCK_MONOTONIC, &end);

            time_deallocating_ns += timespec_diff_ns(&start, &end);
        }
    }

    printf("Time spent allocating\t%lu us\n", time_allocating_ns/ITERATION_COUNT);
    printf("Time spent deallocating\t%lu us\n", time_deallocating_ns/ITERATION_COUNT);
    printf("Time spent in total\t%lu us\n", (time_allocating_ns + time_deallocating_ns)/ITERATION_COUNT);
}
