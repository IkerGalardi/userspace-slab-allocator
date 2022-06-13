#!/bin/sh

FLAGS="-I./src -L. -lslaballocator -lgsl -lgslcblas -lm -ldl -g"

mkdir -p bin

echo "[+] Preparing benchmarks"
echo "  [+] randomsize_fifo_synthetic"
gcc bench/randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic $FLAGS || exit
gcc bench/randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic_smalloc $FLAGS -DUSE_SMALLOC || exit

echo "  [+] randomalloc_fifo_synthetic"
gcc bench/randomalloc_synthetic.c -o bin/randomalloc_synthetic $FLAGS || exit
gcc bench/randomalloc_synthetic.c -o bin/randomalloc_synthetic_smalloc $FLAGS -DUSE_SMALLOC || exit

echo "  [+] cfrac"
cd bench/cfrac
make -f oldMakefile clean &> /dev/null || exit
make -f oldMakefile &> /dev/null || exit
mv cfrac ../../bin/cfrac_sys
make -f oldMakefile clean &> /dev/null || exit
make -f oldMakefile ADDITIONAL_FLAGS=-DUSE_SMALLOC &> /dev/null || exit
mv cfrac ../../bin/cfrac_smalloc
cd ../..

echo "[+] Executing benchmarks"
echo "RANDOM SIZE, FIFO FREE, SYNTHETIC"
echo "  · glibc allocator"
nice -n -20 ./bin/randomsize_fifo_synthetic || exit

echo "  · slabed allocator"
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc || exit

echo "RANDOM PATTERN SYNTHETIC"
echo "  · glibc allocator"
nice -n -20 ./bin/randomalloc_synthetic -d 20 || exit

echo "  · slabed allocator"
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20 || exit

echo
echo "CFRAC"
echo "  · glibc allocator"
time nice -n -20 ./bin/cfrac_sys 4175764634412383261319054216609912102 || exit

echo "  · slabed allocator"
time nice -n -20 ./bin/cfrac_smalloc 4175764634412383261319054216609912102 || exit