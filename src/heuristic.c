#include "heuristic.h"

#include <stdio.h>

#include "utils.h"

int heuristic_decision_grow_count(struct heuristic_params params, struct heuristic_data data) {
    UNUSED_PARAMETER(data);
    return params.default_grow_rate;
}

bool heuristic_decision_does_unmap(struct heuristic_params params, struct heuristic_data data) {
    UNUSED_PARAMETER(params);
    
    float ratio = (float)data.allocation_count / (float)data.deallocation_count;
    
    //printf("Ratio = %f\n", ratio);
    //return true;
    
    if(ratio < 0.3f) {
        return true;
    } else {
        return false;
    }
}
