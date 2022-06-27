#include "heuristic.h"

#include <stdio.h>

#include "utils.h"

int heuristic_decision_grow_count(struct heuristic_params params, struct heuristic_data data) {
    UNUSED_PARAMETER(data);
    return params.default_grow_rate;
}

bool heuristic_decision_does_free_slab(struct heuristic_params params, struct heuristic_data data) {
    size_t empty_slabs = data.grow_count - data.shrink_count;
    return empty_slabs <= params.minimum_empty_slabs;
}
