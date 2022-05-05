#!/bin/sh

mkdir -p bin

FLAGS="-O2 -I../slab_allocator/src -L../slab_allocator -lslaballocator"

echo "SIMPLE SINTHETIC BENCHMARK"
echo "  · glibc allocator"
gcc simple_synthetic.c -o bin/simple_synthetic $FLAGS
time ./bin/simple_synthetic

echo "  · slabed allocator"
gcc simple_synthetic.c -o bin/simple_synthetic -DUSE_SMALLOC $FLAGS
time ./bin/simple_synthetic


