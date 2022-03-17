workspace "Userspace Slab"
    configurations { "Debug", "Release" }

    include "slab_allocator"
    include "slab_tests"
    include "slab_bench"
