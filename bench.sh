cd slab_allocator
make config=release
cd ../slab_bench/
sh compile_all.sh
sh bench.sh
