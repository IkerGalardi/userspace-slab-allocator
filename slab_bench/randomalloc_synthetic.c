#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <smalloc.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#ifdef USE_SMALLOC
    #define allocate(x)   smalloc(x)
    #define deallocate(x) sfree(x)
#else
    #define allocate(x)   malloc(x)
    #define deallocate(x) free(x)
#endif // USE_SMALLOC

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

int BETA_DISTRIBUTION_ALPHA_VALUE = 4;
int BETA_DISTRIBUTION_BETA_VALUE  = 1;

#define ITERATION_COUNT 100000000
#define MAX_ALLOCATIONS 100000

static inline uint64_t timespec_diff_ns(struct timespec *start, struct timespec *end)
{
    uint64_t ret;
    ret =  (uint64_t)(end->tv_sec - start->tv_sec) * CLOCKS_PER_SEC;
    ret += (uint64_t)(end->tv_nsec - start->tv_nsec);
    return ret;
}

struct pointer_array {
    void** pointers;
    size_t pointer_count;
    size_t capacity;
};

bool pointer_array_append(struct pointer_array* array, void* pointer) {
    array->pointers[array->pointer_count] = pointer;
    array->pointer_count++;
}

void pointer_array_remove(struct pointer_array* array, size_t index) {
    for(int i = index; i < (array->pointer_count - 1); i++) {
        array->pointers[i] = array->pointers[i + 1];
    }

    array->pointer_count--;
}

int main() {
    smalloc_initialize();

    int allocation_sizes[] = { 2, 8, 12, 32, 37, 128, 512 };

    struct pointer_array pa;
    pa.pointers = malloc(MAX_ALLOCATIONS * sizeof(void*));
    pa.pointer_count = 0;
    pa.capacity = MAX_ALLOCATIONS;

    gsl_rng_env_setup();
    gsl_rng* r = gsl_rng_alloc (gsl_rng_default);

    uint64_t us_allocating = 0;
    uint64_t us_deallocating = 0;

    uint64_t ns_allocating_tmp = 0;
    uint64_t ns_deallocating_tmp = 0;

    double election_threshold = 2.0;
    for(int it = 0; it < ITERATION_COUNT; it++) {
        // Randombly select if it's needed to malloc or free. 0 for free, 1 for malloc.
        double election = 4 * (((double)(rand() % 100)) / 100.0);

        struct timespec start;
        struct timespec end;

        // If free action and there are pointers to free, do the free
        if(election < election_threshold && pa.pointer_count != 0) {
            int free_index = rand() % pa.pointer_count;
            
            void* pointer = pa.pointers[free_index];

            clock_gettime(CLOCK_MONOTONIC, &start);
            deallocate(pointer);
            clock_gettime(CLOCK_MONOTONIC, &end);

            ns_deallocating_tmp = timespec_diff_ns(&start, &end);

            pointer_array_remove(&pa, free_index);

        // if there aren't pointers to free, then malloc 
        } else if(election < election_threshold && pa.pointer_count == 0) {
            int index = rand() % 7;

            clock_gettime(CLOCK_MONOTONIC, &start);
            void* pointer = allocate(allocation_sizes[index]); 
            clock_gettime(CLOCK_MONOTONIC, &end);

            ns_allocating_tmp = timespec_diff_ns(&start, &end);

            pointer_array_append(&pa, pointer);

        // if malloc action and enogh space on the array, then malloc
        } else if(election >= election_threshold && pa.pointer_count < pa.capacity) {
            int index = rand() % 7;

            clock_gettime(CLOCK_MONOTONIC, &start);
            void* pointer = allocate(allocation_sizes[index]); 
            clock_gettime(CLOCK_MONOTONIC, &end);

            ns_allocating_tmp = timespec_diff_ns(&start, &end);

            pointer_array_append(&pa, pointer);

        // if malloc action and not enough space then free
        } else if(election >= election_threshold && pa.pointer_count == pa.capacity-1) {
            int free_index = rand() % pa.pointer_count;
            
            void* pointer = pa.pointers[free_index];

            clock_gettime(CLOCK_MONOTONIC, &start);
            deallocate(pointer);
            clock_gettime(CLOCK_MONOTONIC, &end);

            ns_deallocating_tmp = timespec_diff_ns(&start, &end);

            pointer_array_remove(&pa, free_index);
        } 

        double full_percentage = (double)pa.pointer_count / ((double)pa.capacity / 5.0);
        election_threshold = gsl_ran_beta_pdf(full_percentage, 4, 1);

        // Update the usecs with the nanoseconds. This way we dont overflow
        if(it % 1000 == 0) {
            us_allocating += ns_allocating_tmp * 0.001;
            us_deallocating += ns_deallocating_tmp * 0.001;

            ns_allocating_tmp = 0;
            ns_deallocating_tmp = 0;
        }
    }

    printf("Time spent allocating\t%lu us\n", us_allocating);
    printf("Time spent deallocating\t%lu us\n", us_deallocating);
    printf("Time spent in total\t%lu us\n", us_deallocating + us_allocating);
}
