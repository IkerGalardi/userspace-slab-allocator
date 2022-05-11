#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <smalloc.h>
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

#define BETA_DISTRIBUTION_ALPHA_VALUE 4
#define BETA_DISTRIBUTION_BETA_VALUE  1

#define ITERATION_COUNT 1000000000
#define MAX_ALLOCATIONS 100000

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

    double election_threshold = 2.0;
    for(int it = 0; it < ITERATION_COUNT; it++) {
        // Randombly select if it's needed to malloc or free. 0 for free, 1 for malloc.
        float x = ((float)pa.pointer_count) / ((float)pa.capacity);
        double beta_random = gsl_ran_beta_pdf(x, BETA_DISTRIBUTION_ALPHA_VALUE, BETA_DISTRIBUTION_BETA_VALUE);

        // If free action and there are pointers to free, do the free
        if(beta_random > election_threshold && pa.pointer_count != 0) {

            int free_index = rand() % pa.pointer_count;
            
            void* pointer = pa.pointers[free_index];
            deallocate(pointer);

            pointer_array_remove(&pa, free_index);

        // if there aren't pointers to free, then malloc 
        } else if(beta_random > election_threshold && pa.pointer_count == 0) {
            int index = rand() % 7;
            void* pointer = allocate(allocation_sizes[index]); 

            pointer_array_append(&pa, pointer);

        // if malloc action and enogh space on the array, then malloc
        } else if(beta_random <= election_threshold && pa.pointer_count < pa.capacity) {
            int index = rand() % 7;
            void* pointer = allocate(allocation_sizes[index]); 

            pointer_array_append(&pa, pointer);

        // if malloc action and not enough space then free
        } else if(beta_random <= election_threshold && pa.pointer_count == pa.capacity-1) {
            int free_index = rand() % pa.pointer_count;
            
            void* pointer = pa.pointers[free_index];
            deallocate(pointer);

            pointer_array_remove(&pa, free_index);
        } 

        // Randomly select by how much move the election_threshold
        int random_uniform = rand() % 100 - 50;
        double how_much_move = ((double)random_uniform) / 60;

        // Move and clamp values between 1 and 2
        election_threshold += how_much_move;
        election_threshold = MIN(2, election_threshold);
        election_threshold = MAX(1, election_threshold);
    }
}
