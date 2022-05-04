#ifndef INTERNAL_ASSERT_H
#define INTERNAL_ASSERT_H

// Doing this in order to remove unnecessary function calls inside the allocator. Those
// functions are called in order to check the correct functioning of the allocator, but
// are not necessary in release builds.
#ifdef DEBUG_ASSERTS
    #include <assert.h>
    #define assert_not_reached() assert((false))
#else
    #define assert(x)
    #define assert_not_reached()
#endif // SLAB_DEBUG

#endif // INTERNAL_ASSERT_H
