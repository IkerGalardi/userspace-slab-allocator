#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

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

#define MAX_ALLOCATION_SIZE 8000

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

#define OP_UNKNOWN 0
#define OP_MALLOC  1
#define OP_FREE    2

struct operation {
    size_t operation_type;
    size_t free_index;
    int* operand;
};

struct benchmark_parameters {
    // Size distribution parameters
    int alpha;
    int beta;

    // Pattern parameters
    size_t average_spacing;
};

struct benchmark_parameters get_params(int argc, char** argv) {
    struct benchmark_parameters result = {
        .alpha = 2,
        .beta = 20,
        .average_spacing = 4
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

        case 'd':
            result.average_spacing = atoi(optarg);
            break;
        }
    }

    return result;
}

struct operation* construct_operation_array(size_t count, size_t average_spacing) {
    struct operation* operations = (struct operation*)calloc(count, sizeof(struct operation));

    struct operation malloc_operation;
    struct operation free_operation;
    for(int i = 0; i < count; i++) {
        if(operations[i].operation_type != OP_UNKNOWN)
            continue;

        // Create the malloc operation and matching free operation
        malloc_operation.operation_type = OP_MALLOC;
        malloc_operation.free_index = i + average_spacing;
        malloc_operation.operand = NULL;

        free_operation.operation_type = OP_FREE;
        free_operation.free_index = 0;
        free_operation.operand = NULL;

        // Add the operations to the array
        memcpy(operations + i, &malloc_operation, sizeof(struct operation));

        if(i + average_spacing < count) {
            memcpy(operations + i + average_spacing, &free_operation, sizeof(struct operation));
        }
    }

    return operations;
}

void print_operation_list(struct operation* ops, size_t count) {
    for(int i = 0; i < count; i++) {
        if(ops[i].operation_type == OP_MALLOC) {
            
        } else if(ops[i].operation_type == OP_MALLOC) {

        }
    }
}

int main(int argc, char** argv) {
    struct benchmark_parameters params = get_params (argc, argv);

    struct timespec start;
    struct timespec end;
    size_t allocation_time = 0;
    size_t deallocation_time = 0;

    size_t operation_count = 100000000;
    struct operation* ops = construct_operation_array(operation_count, params.average_spacing);

    gsl_rng* r = gsl_rng_alloc(gsl_rng_default);
    gsl_rng_set(r, 0);

    for(int i = 0; i < operation_count; i++) {
        struct operation op = ops[i];
        if(op.operation_type == OP_MALLOC) {
            // If the free is out of bounds, simply ignore.
            if(op.free_index >= operation_count)
                continue;

            size_t allocation_size = roundf(MAX_ALLOCATION_SIZE *
                                         gsl_ran_beta(r, params.alpha, params.beta));

            struct operation* matching_free_op = ops + op.free_index;

            clock_gettime(CLOCK_MONOTONIC, &start);
            matching_free_op->operand = allocate(10);
            clock_gettime(CLOCK_MONOTONIC, &end);
            
            allocation_time += timespec_diff_ns(&start, &end);

        } else if(op.operation_type == OP_FREE) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            deallocate(op.operand);
            clock_gettime(CLOCK_MONOTONIC, &end);

            deallocation_time += timespec_diff_ns(&start, &end);
        }
    }

    printf("Allocation time: %lu\n", allocation_time);
    printf("Dellocation time: %lu\n", deallocation_time);
}
