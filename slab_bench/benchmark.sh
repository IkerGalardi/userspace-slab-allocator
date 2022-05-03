#!/bin/sh

set -v

make config=release || exit

ALLOCATION_COUNT=100000000

echo "Alloc size,Allocation times,Deallocation times" > system.csv
./bench -t system -s 100 -c $ALLOCATION_COUNT > /dev/null || exit
./bench -t system -s 8   -c $ALLOCATION_COUNT >> system.csv || exit
./bench -t system -s 16  -c $ALLOCATION_COUNT >> system.csv || exit
./bench -t system -s 32  -c $ALLOCATION_COUNT >> system.csv || exit
./bench -t system -s 64  -c $ALLOCATION_COUNT >> system.csv || exit
./bench -t system -s 128 -c $ALLOCATION_COUNT >> system.csv || exit

echo "Alloc size,Allocation times,Deallocation times" > slab.csv
./bench -t slab -s 100 -c $ALLOCATION_COUNT > /dev/null || exit
./bench -t slab -s 8   -c $ALLOCATION_COUNT >> slab.csv || exit
./bench -t slab -s 16  -c $ALLOCATION_COUNT >> slab.csv || exit
./bench -t slab -s 32  -c $ALLOCATION_COUNT >> slab.csv || exit
./bench -t slab -s 64  -c $ALLOCATION_COUNT >> slab.csv || exit
./bench -t slab -s 128 -c $ALLOCATION_COUNT >> slab.csv || exit

python3 charts.py system.csv slab.csv
