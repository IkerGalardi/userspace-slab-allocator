#include <time.h>
#include <stdlib.h>

#include <smalloc.h>

#ifdef USE_SMALLOC
    #define allocate(x)   smalloc(x)
    #define deallocate(x) sfree(x)
#else
    #define allocate(x)   malloc(x)
    #define deallocate(x) free(x)
#endif // USE_SMALLOC

#define ITERATION_COUNT 500
#define ALLOCATION_COUNT 100000

int main() {
    smalloc_initialize();

    int allocation_sizes[] = { 2, 8, 12, 32, 37, 128, 512 };

    void** allocation = malloc(ALLOCATION_COUNT * sizeof(void*));

    for(int it = 0; it < ITERATION_COUNT; it++) {
        for(int i = 0; i < ALLOCATION_COUNT; i++) {
            int index = rand() % 7;
            allocation[i] = allocate(allocation_sizes[index]); 
        }

        for(int i = 0; i < ALLOCATION_COUNT; i++) {
            deallocate(allocation[i]); 
        }
    }
}
