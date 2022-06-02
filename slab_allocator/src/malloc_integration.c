#include "smalloc.h"

#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// TODO: remove GNU specific stuff. Should be cross os
#define __USE_GNU
#include <dlfcn.h>

#include "malloc_integration.h"

//#define SMALLOC_CONFIG_CAMOUFLAGE
#ifdef SMALLOC_CONFIG_CAMOUFLAGE
    #pragma message "The malloc function will be replaced"
#endif // SMALLOC_CONFIG_CAMOUFLAGE

malloc_function system_malloc;
free_function system_free;

void __attribute__((constructor)) setup_smalloc() {
#ifdef SMALLOC_CONFIG_CAMOUFLAGE
    smalloc_initialize();
#endif // SMALLOC_CONFIG_CAMOUFLAGE

    system_malloc = (malloc_function)dlsym(RTLD_NEXT, "malloc");
    system_free = (free_function)dlsym(RTLD_NEXT, "free");

    assert((system_malloc != NULL));
    assert((system_free != NULL));
}

#ifdef SMALLOC_CONFIG_CAMOUFLAGE
void* malloc(size_t size) {
    return smalloc(size);
}

void free(void* ptr) {
    sfree(ptr);
}
#endif // SMALLOC_CONFIG_CAMOUFLAGE
