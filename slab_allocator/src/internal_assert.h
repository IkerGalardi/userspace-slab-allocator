#ifndef INTERNAL_ASSERT_H
#define INTERNAL_ASSERT_H

// Doing this in order to remove unnecessary function calls inside the allocator. Those
// functions are called in order to check the correct functioning of the allocator, but
// are not necessary in release builds.
#ifdef SLAB_DEBUG
    #include <assert.h>
#else
    #define assert(x)
#endif // SLAB_DEBUG

#endif // INTERNAL_ASSERT_H
