#ifndef SLAB_HEURISTIC_H
#define SLAB_HEURISTIC_H

#include <stdbool.h>
#include <stddef.h>

// TODO: temporal information would be good for more advanced 
//       heuristics.

struct heuristic_params {
    size_t minimum_empty_slabs;
    size_t default_grow_rate;
    size_t max_grow_rate;
};

struct heuristic_data {
    size_t allocation_count;
    size_t deallocation_count;
    size_t total_count;
    size_t full_count;
};

int heuristic_decision_grow_count(struct heuristic_params params, struct heuristic_data data);

bool heuristic_decision_does_unmap(struct heuristic_params params, struct heuristic_data data);

#endif // SLAB_HEURISTIC_H
