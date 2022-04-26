#!/bin/sh

make config=release || exit

# Execute twice, take the results of the second one
./bench system > /dev/null  || exit
./bench system > system.csv || exit

# Execute twice, take the results of the second one
./bench slab   > /dev/null  || exit
./bench slab   > slab.csv   || exit

# Graph the results
python3 charts.py system.csv slab.csv
