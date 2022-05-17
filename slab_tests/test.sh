#!/bin/sh

FLAGS="-O2 -I../slab_allocator/src -L../slab_allocator -lslaballocator -lgsl -lgslcblas"

# Reset
Color_Off='\033[0m'       # Text Reset

# Regular Colors
Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White

mkdir -p bin

echo "[+] Building 'slab' test"
gcc slab_test.c  -o bin/slab_test $FLAGS || exit 

echo "[+] Building 'pool' test"
gcc pool_test.c  -o bin/pool_test $FLAGS || exit 

echo "[+] Building 'smalloc' test"
gcc smalloc_test.c  -o bin/smalloc_test $FLAGS || exit 

mkdir -p logs

echo
echo "[+] Testing 'slab'"
./bin/slab_test &> logs/smalloc_test.log
if [ $? -ne 0 ] 
then
    echo -e "$Red FAILED: slab test failed $ColorOff"
    exit
else
    echo -e "$Green PASSED: slab test passed $Color_Off"
fi

echo "[+] Testing 'pool'"
./bin/pool_test &> logs/smalloc_test.log
if [ $? -ne 0 ] 
then
    echo -e "$Red FAILED: pool test failed $ColorOff"
    exit
else 
    echo -e "$Green PASSED: pool test passed $Color_Off"
fi

echo "[+] Testing 'smalloc'"
./bin/smalloc_test &> logs/smalloc_test.log
if [ $? -ne 0 ] 
then
    echo -e "$Red FAILED: smalloc test failed $Color_Off"
    exit
else
    echo -e "$Green PASSED: smalloc test passed $Color_Off"
fi