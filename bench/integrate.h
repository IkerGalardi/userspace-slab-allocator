#ifndef INTEGRATE_H
#define INTEGRATE_H

#ifdef USE_SMALLOC
    #include <smalloc.h>
    #define allocate(x) smalloc(x);
    #define reallocate(x, y) srealloc(x, y);
    #define deallocate(x) sfree(x);
#else
    #include <stdlib.h>
    #define allocate(x) malloc(x);
    #define reallocate(x, y) realloc(x, y);
    #define deallocate(x) free(x);
#endif // USE_SMALLOC

#endif // INTEGRATE_H
