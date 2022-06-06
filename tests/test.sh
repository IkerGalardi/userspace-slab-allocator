#!/bin/sh

FLAGS="-O2 -I./src/ -L. -lslaballocator -lgsl -lgslcblas -ldl -g"

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

echo "[+] Building 'slab' test"
gcc tests/slab_test.c  -o bin/slab_test $FLAGS || exit 

echo "[+] Building 'slab_create_several' test"
gcc tests/slab_create_several_test.c  -o bin/slab_create_several_test $FLAGS || exit 

echo "[+] Building 'pool' test"
gcc tests/pool_test.c  -o bin/pool_test $FLAGS || exit 

echo "[+] Building 'smalloc' test"
gcc tests/smalloc_test.c  -o bin/smalloc_test $FLAGS || exit 

mkdir -p tests/logs

echo
echo "[+] Testing 'slab'"
./bin/slab_test &> tests/logs/slab_test.log
if [ $? -ne 0 ] 
then
    echo -e "$Red FAILED: slab test failed $ColorOff"
    exit
else
    echo -e "$Green PASSED: slab test passed $Color_Off"
fi

echo "[+] Testing 'slab_create_several'"
./bin/slab_create_several_test &> tests/logs/slab_create_several_test.log
if [ $? -ne 0 ] 
then
    echo -e "$Red FAILED: slab_create_several test failed $ColorOff"
    exit
else 
    echo -e "$Green PASSED: slab_create_several test passed $Color_Off"
fi

echo "[+] Testing 'pool'"
./bin/pool_test &> tests/logs/pool_test.log
if [ $? -ne 0 ] 
then
    echo -e "$Red FAILED: pool test failed $ColorOff"
    exit
else 
    echo -e "$Green PASSED: pool test passed $Color_Off"
fi

echo "[+] Testing 'smalloc'"
./bin/smalloc_test &> tests/logs/smalloc_test.log
if [ $? -ne 0 ] 
then
    echo -e "$Red FAILED: smalloc test failed $Color_Off"
    exit
else
    echo -e "$Green PASSED: smalloc test passed $Color_Off"
fi

echo "[+] Testing malloc integration"
./bin/integration_test &> tests/logs/integration_test.log
if [ $? -ne 0 ]
then
    echo -e "$Red FAILED: integration test failed $Color_Off"
    exit
else
    echo -e "$Green PASSED: test passed $Color_Off"
fi