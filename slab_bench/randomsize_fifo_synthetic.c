#include <time.h>
#include <stdlib.h>

#include <smalloc.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#ifdef USE_SMALLOC
    #define allocate(x)   smalloc(x)
    #define deallocate(x) sfree(x)
#else
    #define allocate(x)   malloc(x)
    #define deallocate(x) free(x)
#endif // USE_SMALLOC

#define ITERATION_COUNT 1
#define ALLOCATION_COUNT 100000

#define MIN_ALLOCATION_SIZE 1
#define MAX_ALLOCATION_SIZE 800

#define ALPHA 2
#define BETA  20

int main() {
    smalloc_initialize();

    gsl_rng* r = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(r, 0);

    void** allocation = malloc(ALLOCATION_COUNT * sizeof(void*));

    for(int it = 0; it < ITERATION_COUNT; it++) {
        for(int i = 0; i < ALLOCATION_COUNT; i++) {
            int allocation_size = roundf(MAX_ALLOCATION_SIZE * gsl_ran_beta(r, ALPHA, BETA));
            allocation[i] = allocate(allocation_size); 
        }

        for(int i = 0; i < ALLOCATION_COUNT; i++) {
            deallocate(allocation[i]); 
        }
    }
}
