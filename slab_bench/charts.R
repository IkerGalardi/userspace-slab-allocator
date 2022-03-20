csv_file = read.csv("results/2022-03-20.csv")

alloc_sizes = csv_file$Alloc.size
sys_malloc = csv_file$System.malloc
sys_free = csv_file$System.free
slabbed_malloc = csv_file$Slabbed.malloc
slabbed_free = csv_file$Slabbed.free