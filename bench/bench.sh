#!/bin/sh

FLAGS="-I./src -L. -lslaballocator -lgsl -lgslcblas -lm -ldl -g"

mkdir -p bin

echo "[+] randomsize_fifo_synthetic"
gcc bench/randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic $FLAGS || exit
echo "[+] randomsize_fifo_synthetic_smalloc"
gcc bench/randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic_smalloc $FLAGS -DUSE_SMALLOC || exit

echo "RANDOM SIZE, FIFO FREE, SYNTHETIC"
echo "  · glibc allocator"
./bin/randomsize_fifo_synthetic || exit

echo "  · slabed allocator"
./bin/randomsize_fifo_synthetic_smalloc || exit