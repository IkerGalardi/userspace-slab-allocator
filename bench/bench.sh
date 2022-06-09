#!/bin/sh

FLAGS="-I./src -L. -lslaballocator -lgsl -lgslcblas -lm -ldl -g"

mkdir -p bin

echo "[+] Preparing benchmarks"
echo "  [+] randomsize_fifo_synthetic"
gcc bench/randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic $FLAGS || exit

echo "  [+] randomsize_fifo_synthetic_smalloc"
gcc bench/randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic_smalloc $FLAGS -DUSE_SMALLOC || exit

echo "  [+] cfrac"
cd bench/cfrac
make -f oldMakefile clean
make -f oldMakefile
mv cfrac ../../bin/cfrac_sys
make -f oldMakefile clean
make -f oldMakefile ADDITIONAL_FLAGS=-DUSE_SMALLOC
mv cfrac ../../bin/cfrac_smalloc
cd ..

echo "RANDOM SIZE, FIFO FREE, SYNTHETIC"
echo "  · glibc allocator"
nice -n -20 ./bin/randomsize_fifo_synthetic || exit

echo "  · slabed allocator"
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc || exit