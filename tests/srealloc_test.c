#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <smalloc.h>

#define INITIAL_SIZE 5
#define REALLOC_SIZE 10

static void print_array(int size, int* a) {
    for(int i = 0; i < size; i++) {
        printf("%d, ", a[i]);
    }
    fflush(stdout);
}

static bool compare_arrays(int size, int* a, int* b) {
    for(int i = 0; i < size; i++) {
        if(a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

int main(int argc, char** argv) {
    smalloc_initialize();

    int expected_result[REALLOC_SIZE] = {
        1, 1, 1, 1, 1,
        2, 2, 2, 2, 2
    };

    int* initial = smalloc(INITIAL_SIZE * sizeof(int));
    for(int i = 0; i < INITIAL_SIZE; i++) {
        initial[i] = 1;
    }

    int* reallocated = srealloc(initial, REALLOC_SIZE * sizeof(int));
    for(int i = INITIAL_SIZE; i < REALLOC_SIZE; i++) {
        reallocated[i] = 2;
    }

    printf("Expected array: "); print_array(REALLOC_SIZE, expected_result); printf("\n");
    printf("Reallocated array: "); print_array(REALLOC_SIZE, reallocated); printf("\n");
    assert(compare_arrays(REALLOC_SIZE, expected_result, reallocated));
}
