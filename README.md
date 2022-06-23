# userspace-slab-allocator

Slab allocator implemented for userspace in order to test if the performance improvements in the kernel world translate to userland.

## Building and using the allocator
The project does not depend on any external libraries, so nothing apart from the system toolchain (gcc, ld, make, etc) is needed. To compile just execute `make` on the root of the project, that should compile and build a static library named *libslaballocator.a*. **WARNING: the allocator is not ready for production, use with caution.**

## Hacking on the allocator
This project only serves as investigation, it's just a prototype. Development will stop once my final degree project ends so contributions won't be merged. If instead you wan't to integrate it to your allocator, you can read the documentation (*COMING SOON*) or simply exploring the code (I recommend starting with `smalloc.c` and explore the structures used).

## Executing the benchmarks
For executing the benchmark, just type the next command:

```bash
    make benchmark
```

If the command is executed with enough priviledges, it will try to run the benchmarks with maximum priority (in order to reduce OS noise). If not, don't worry about the errors, it's simply telling that the `nice` command could not elevate priority on the benchmark process, it will execute fine.
