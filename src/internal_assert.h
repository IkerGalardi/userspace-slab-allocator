#ifndef INTERNAL_ASSERT_H
#define INTERNAL_ASSERT_H

// Asserts will always be available on debug builds.
#ifdef SLAB_DEBUG
    #define DEBUG_ASSERTS
#endif // SLAB_DEBUG

// Doing this in order to remove unnecessary function calls inside the allocator. Those
// functions are called in order to check the correct functioning of the allocator, but
// are not necessary in release builds.
#ifdef DEBUG_ASSERTS
    #include <assert.h>
    #define assert_not_reached() assert((false))
    #pragma message "Build might be slower, asserts are currently active"
#else
    #define assert(x)
    #define assert_not_reached()
#endif // SLAB_DEBUG

#endif // INTERNAL_ASSERT_H
