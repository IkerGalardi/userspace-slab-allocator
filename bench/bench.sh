#!/bin/sh

FLAGS="-I./src -L. -lslaballocator -lgsl -lgslcblas -lm -ldl -g"

mkdir -p bin

echo "[+] Preparing benchmarks"
echo "  [+] randomsize_fifo_synthetic"
gcc bench/randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic $FLAGS || exit

echo "  [+] randomsize_fifo_synthetic_smalloc"
gcc bench/randomsize_fifo_synthetic.c -o bin/randomsize_fifo_synthetic_smalloc $FLAGS -DUSE_SMALLOC || exit

echo "  [+] Cloning dash and preparing environment"
rm -rf bench/dash
cd bench/
git clone https://github.com/tklauser/dash.git &> dash.log || exit
cd dash
sh autogen.sh &> ../dash.log || exit
./configure &> ../dash.log || exit
make > ../dash.log|| exit
mv src/dash ../../bin/dash_system
sed -i src/memmaloc.h '

echo "RANDOM SIZE, FIFO FREE, SYNTHETIC"
echo "  · glibc allocator"
./bin/randomsize_fifo_synthetic || exit

echo "  · slabed allocator"
./bin/randomsize_fifo_synthetic_smalloc || exit