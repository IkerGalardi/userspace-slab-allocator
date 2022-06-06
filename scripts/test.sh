#!/bin/sh

cd slab_allocator
make config=testing || exit
cd ../slab_tests/
sh test.sh
