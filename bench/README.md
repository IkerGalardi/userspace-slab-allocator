# Benchmarks

This folder has all the source code/projects used to benchmark the allocator. The next are the available benchmarks:

* **randomalloc_synthetic**: given the average distance between a malloc and it's respective free (`-d` flag), it constructs an array of operations and goes executing those operations and timing what cost each allocation/deallocation has.

* **randomsize_fifo_synthetic**: given the parameters for the beta distribution (`-a` and `-b` flags), it chooses the allocation size. Used to benchmark performance benefits on favorable sizes and performance loss when slabs are not used.

* **cfrac**: cfrac is a general purpose factorization algorithm that has many small temporal allocations (theoretically benefitial to the slab allocator).