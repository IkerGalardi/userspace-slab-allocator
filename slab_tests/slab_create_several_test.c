#include <stdio.h>
#include <assert.h>

#include <slab.h>

static int get_list_size(struct mem_slab* list_start) {
    struct mem_slab* current = list_start;
    int jumps = 0;

    while(current != NULL) {
        jumps++;
        current = current->next;
    }

    return jumps;
}

int main(int argc, char** argv) {
    struct mem_slab* a = mem_slab_create_several(7, 0, 9, NULL);
    assert((get_list_size(a) == 9));

    struct mem_slab* b = mem_slab_create_several(7, 0, 10, a);
    assert((get_list_size(b) == 19));
}