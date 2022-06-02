#include <stdlib.h>
#include <stdio.h>

#include <smalloc.h>

int main(int argc, char** argv) {
    void* pointer = malloc(7);

    if(pointer == NULL)
        return 1;

    free(pointer);
}
