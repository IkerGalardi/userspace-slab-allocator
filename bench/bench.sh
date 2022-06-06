#!/bin/sh

mkdir -p bin

FLAGS="-O2 -I../slab_allocator/src -L../slab_allocator -lslaballocator"

echo "RANDOM SIZE, FIFO FREE, SYNTHETIC"
echo "  · glibc allocator"
#./bin/randomsize_fifo_synthetic || exit

echo "  · slabed allocator"
./bin/randomsize_fifo_synthetic_smalloc || exit

echo
echo "RANDOM ALLOC/FREE, SYNTHETIC"
echo "  · glibc allocator"
./bin/randomalloc_synthetic || exit

echo "  · slabed allocator"
./bin/randomalloc_synthetic_smalloc || exit
