#!/bin/sh

make config=release || exit

echo "Alloc size,Allocation times,Deallocation times" > system.csv
./bench -t system -s 100 > /dev/null || exit
./bench -t system -s 8   >> system.csv || exit
./bench -t system -s 16  >> system.csv || exit
./bench -t system -s 32  >> system.csv || exit
./bench -t system -s 64  >> system.csv || exit
./bench -t system -s 128 >> system.csv || exit

echo "Alloc size,Allocation times,Deallocation times" > slab.csv
./bench -t slab -s 100 > /dev/null || exit
./bench -t slab -s 8   >> slab.csv || exit
./bench -t slab -s 16  >> slab.csv || exit
./bench -t slab -s 32  >> slab.csv || exit
./bench -t slab -s 64  >> slab.csv || exit
./bench -t slab -s 128 >> slab.csv || exit

python3 charts.py system.csv slab.csv
