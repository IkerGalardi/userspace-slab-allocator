CC=gcc
AR=ar
CFLAGS=-march=native -O3 -g -Wall -Werror -Wpedantic -Wextra
ADDITIONAL_FLAGS=

LIBRARY_NAME=libslaballocator

OBJECT_FILES=bin/slab_pool.o \
             bin/slab.o \
             bin/smalloc.o \
             bin/utils.o \
             bin/heuristic.o

all: static

shared:
	@ echo "NOT SUPPORTED RIGHT NOW"
	@ exit 4

static: $(OBJECT_FILES)
	$ $(AR) -rcs $(LIBRARY_NAME).a $(OBJECT_FILES)

bin/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS) $(ADDITIONAL_FLAGS)

test: static
	make clean
	make ADDITIONAL_FLAGS="-DSLAB_DEBUG -DDSLAB_CONFIG_PARANOID_ASSERTS -DPOOL_CONFIG_PARANOID_ASSERTS"
	bash scripts/test.sh

install: static
	cp $(LIBRARY_NAME).* /usr/lib64
	cp src/*.h /usr/include

benchmark: static
	bash scripts/bench.sh

clean:
	# Clean build files
	@ rm bin/*

	# Clean benchmark files
	cd bench/cfrac
	make -C bench/cfrac -f oldMakefile clean

help:
	@echo "Targets:"
	@echo " · static: build a static library"
	@echo " · shared: build a shared library"
	@echo " · test: build a debugeable static library and test"
	@echo " · benchmark: build an optimized library and execute the benchmarks"
	@echo ""
	@echo "ADDITIONAL_FLAGS:"
	@echo " · -DSLAB_DEBUG: enable debugging prints"
	@echo " · -DSLAB_CONFIG_PARANOID_ASSERTS: enable paranoid asserts, testing stuff that should work but maybe doesn't"
	@echo " · -DPOOL_CONFIG_PARANOID_ASSERTS: enable paranoid asserts, testing stuff that should work but maybe doesn't"
