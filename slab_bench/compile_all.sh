#!/bin/sh

FLAGS="-O2 -I../slab_allocator/src -L../slab_allocator -lslaballocator -lgsl -lgslcblas -lm -ldl"

mkdir -p bin

echo "[+] randomsize_fifo_synthetic"
gcc randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic $FLAGS || exit
echo "[+] randomsize_fifo_synthetic_smalloc"
gcc randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic_smalloc $FLAGS -DUSE_SMALLOC || exit

echo "[+] randomalloc_synthetic"
gcc randomalloc_synthetic.c -o bin/randomalloc_synthetic $FLAGS || exit
echo "[+] randomalloc_synthetic_smalloc"
gcc randomalloc_synthetic.c -o bin/randomalloc_synthetic_smalloc $FLAGS -DUSE_SMALLOC || exit
