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
    size_t grow_count;
    size_t shrink_count;
};

/*
 * Returns how much the slab pool should grow given the parameters and slab pool statistics.
 */
int heuristic_decision_grow_count(struct heuristic_params params, struct heuristic_data data);

/*
 * Returns whether a slab should be removed or not based on the given parameters and pool statistics
 */
bool heuristic_decision_does_unmap(struct heuristic_params params, struct heuristic_data data);

#endif // SLAB_HEURISTIC_H
