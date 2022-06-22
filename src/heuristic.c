#include "heuristic.h"

#include <stdio.h>

#include "utils.h"

int heuristic_decision_grow_count(struct heuristic_params params, struct heuristic_data data) {
    size_t difference = data.allocation_count - data.deallocation_count;
    return MIN(params.default_grow_rate + difference, params.max_grow_rate);
}

bool heuristic_decision_does_unmap(struct heuristic_params params, struct heuristic_data data) {
    size_t empty_slabs = data.total_count - data.full_count;
    return empty_slabs > params.minimum_empty_slabs;
}
