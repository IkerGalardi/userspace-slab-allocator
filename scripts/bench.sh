#!/bin/sh

FLAGS="-I./src -L. -lslaballocator -lgsl -lgslcblas -lm -ldl -g"

mkdir -p bin

echo "[+] Preparing benchmarks"
echo "  [+] randomsize_fifo_synthetic"
gcc bench/randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic $FLAGS || exit
gcc bench/randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic_smalloc $FLAGS -DUSE_SMALLOC || exit

gcc bench/timerep.c -o bin/timerep $FLAGS || exit

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
echo "  · glibc allocator a = 2, b = 20"
echo "allocations,deallocations" > bench_a2b20_sys.csv
nice -n -20 ./bin/randomsize_fifo_synthetic -a 2 -b 20 >> bench_a2b20_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 2 -b 20 >> bench_a2b20_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 2 -b 20 >> bench_a2b20_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 2 -b 20 >> bench_a2b20_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 2 -b 20 >> bench_a2b20_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 2 -b 20 >> bench_a2b20_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 2 -b 20 >> bench_a2b20_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 2 -b 20 >> bench_a2b20_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 2 -b 20 >> bench_a2b20_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 2 -b 20 >> bench_a2b20_sys.csv || exit

echo "  · glibc allocator a = 10, b = 10"
echo "allocations,deallocations" > bench_a10b10_sys.csv
nice -n -20 ./bin/randomsize_fifo_synthetic -a 10 -b 10 >> bench_a10b10_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 10 -b 10 >> bench_a10b10_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 10 -b 10 >> bench_a10b10_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 10 -b 10 >> bench_a10b10_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 10 -b 10 >> bench_a10b10_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 10 -b 10 >> bench_a10b10_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 10 -b 10 >> bench_a10b10_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 10 -b 10 >> bench_a10b10_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 10 -b 10 >> bench_a10b10_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 10 -b 10 >> bench_a10b10_sys.csv || exit

echo "  · glibc allocator a = 20, b = 2"
echo "allocations,deallocations" > bench_a20b2_sys.csv
nice -n -20 ./bin/randomsize_fifo_synthetic -a 20 -b 2 >> bench_a20b2_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 20 -b 2 >> bench_a20b2_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 20 -b 2 >> bench_a20b2_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 20 -b 2 >> bench_a20b2_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 20 -b 2 >> bench_a20b2_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 20 -b 2 >> bench_a20b2_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 20 -b 2 >> bench_a20b2_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 20 -b 2 >> bench_a20b2_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 20 -b 2 >> bench_a20b2_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 20 -b 2 >> bench_a20b2_sys.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic -a 20 -b 2 >> bench_a20b2_sys.csv || exit

echo "  · slab allocator a = 2, b = 20"
echo "allocations,deallocations" > bench_a2b20_smalloc.csv
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 2 -b 20 >> bench_a2b20_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 2 -b 20 >> bench_a2b20_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 2 -b 20 >> bench_a2b20_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 2 -b 20 >> bench_a2b20_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 2 -b 20 >> bench_a2b20_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 2 -b 20 >> bench_a2b20_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 2 -b 20 >> bench_a2b20_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 2 -b 20 >> bench_a2b20_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 2 -b 20 >> bench_a2b20_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 2 -b 20 >> bench_a2b20_smalloc.csv || exit

echo "  · slab allocator a = 10, b = 10"
echo "allocations,deallocations" > bench_a10b10_smalloc.csv
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 10 -b 10 >> bench_a10b10_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 10 -b 10 >> bench_a10b10_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 10 -b 10 >> bench_a10b10_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 10 -b 10 >> bench_a10b10_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 10 -b 10 >> bench_a10b10_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 10 -b 10 >> bench_a10b10_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 10 -b 10 >> bench_a10b10_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 10 -b 10 >> bench_a10b10_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 10 -b 10 >> bench_a10b10_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 10 -b 10 >> bench_a10b10_smalloc.csv || exit

echo "  · slab allocator a = 20, b = 2"
echo "allocations,deallocations" > bench_a20b2_smalloc.csv
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 20 -b 2 >> bench_a20b2_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 20 -b 2 >> bench_a20b2_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 20 -b 2 >> bench_a20b2_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 20 -b 2 >> bench_a20b2_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 20 -b 2 >> bench_a20b2_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 20 -b 2 >> bench_a20b2_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 20 -b 2 >> bench_a20b2_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 20 -b 2 >> bench_a20b2_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 20 -b 2 >> bench_a20b2_smalloc.csv || exit
nice -n -20 ./bin/randomsize_fifo_synthetic_smalloc -a 20 -b 2 >> bench_a20b2_smalloc.csv || exit


echo "RANDOM PATTERN SYNTHETIC"
echo "  · glibc allocator d = 20"
echo "allocations,deallocations" > bench_d20_sys.csv
nice -n -20 ./bin/randomalloc_synthetic -d 20 >> bench_d20_sys.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic -d 20 >> bench_d20_sys.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic -d 20 >> bench_d20_sys.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic -d 20 >> bench_d20_sys.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic -d 20 >> bench_d20_sys.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic -d 20 >> bench_d20_sys.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic -d 20 >> bench_d20_sys.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic -d 20 >> bench_d20_sys.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic -d 20 >> bench_d20_sys.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic -d 20 >> bench_d20_sys.csv|| exit

echo "  · glibc allocator d = 200"
echo "allocations,deallocations" > bench_d200_sys.csv
nice -n -20 ./bin/randomalloc_synthetic -d 200 >> bench_d200_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 200 >> bench_d200_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 200 >> bench_d200_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 200 >> bench_d200_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 200 >> bench_d200_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 200 >> bench_d200_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 200 >> bench_d200_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 200 >> bench_d200_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 200 >> bench_d200_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 200 >> bench_d200_sys.csv || exit

echo "  · glibc allocator d = 2000"
echo "allocations,deallocations" > bench_d2000_sys.csv
nice -n -20 ./bin/randomalloc_synthetic -d 2000 >> bench_d2000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 2000 >> bench_d2000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 2000 >> bench_d2000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 2000 >> bench_d2000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 2000 >> bench_d2000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 2000 >> bench_d2000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 2000 >> bench_d2000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 2000 >> bench_d2000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 2000 >> bench_d2000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 2000 >> bench_d2000_sys.csv || exit

echo "  · glibc allocator d = 20000"
echo "allocations,deallocations" > bench_d20000_sys.csv
nice -n -20 ./bin/randomalloc_synthetic -d 20000 >> bench_d20000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 20000 >> bench_d20000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 20000 >> bench_d20000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 20000 >> bench_d20000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 20000 >> bench_d20000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 20000 >> bench_d20000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 20000 >> bench_d20000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 20000 >> bench_d20000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 20000 >> bench_d20000_sys.csv || exit
nice -n -20 ./bin/randomalloc_synthetic -d 20000 >> bench_d20000_sys.csv || exit

echo "  · slab allocator d = 20"
echo "allocations,deallocations" > bench_d20_smalloc.csv
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20 >> bench_d20_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20 >> bench_d20_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20 >> bench_d20_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20 >> bench_d20_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20 >> bench_d20_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20 >> bench_d20_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20 >> bench_d20_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20 >> bench_d20_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20 >> bench_d20_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20 >> bench_d20_smalloc.csv|| exit

echo "  · slab allocator d = 200"
echo "allocations,deallocations" > bench_d200_smalloc.csv
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 200 >> bench_d200_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 200 >> bench_d200_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 200 >> bench_d200_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 200 >> bench_d200_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 200 >> bench_d200_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 200 >> bench_d200_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 200 >> bench_d200_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 200 >> bench_d200_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 200 >> bench_d200_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 200 >> bench_d200_smalloc.csv|| exit

echo "  · slab allocator d = 2000"
echo "allocations,deallocations" > bench_d2000_smalloc.csv
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 2000 >> bench_d2000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 2000 >> bench_d2000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 2000 >> bench_d2000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 2000 >> bench_d2000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 2000 >> bench_d2000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 2000 >> bench_d2000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 2000 >> bench_d2000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 2000 >> bench_d2000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 2000 >> bench_d2000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 2000 >> bench_d2000_smalloc.csv|| exit

echo "  · slab allocator d = 20000"
echo "allocations,deallocations" > bench_d20000_smalloc.csv
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20000 >> bench_d20000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20000 >> bench_d20000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20000 >> bench_d20000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20000 >> bench_d20000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20000 >> bench_d20000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20000 >> bench_d20000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20000 >> bench_d20000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20000 >> bench_d20000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20000 >> bench_d20000_smalloc.csv|| exit
nice -n -20 ./bin/randomalloc_synthetic_smalloc -d 20000 >> bench_d20000_smalloc.csv|| exit

echo
echo "CFRAC"
echo "  · glibc allocator"
./bin/timerep nice -n -20 ./bin/cfrac_sys 4175764634412383261319054216609912102 > bench_cfrac_sys.csv || exit

echo "  · slabed allocator"
./bin/timerep nice -n -20 ./bin/cfrac_smalloc 4175764634412383261319054216609912102 bench_crac_sys.csv|| exit