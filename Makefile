CC=gcc
AR=ar
CFLAGS+=-march=native -O2 -g
ADDITIONAL_FLAGS=""

LIBRARY_NAME=libslaballocator

OBJECT_FILES=bin/slab_pool.o \
             bin/slab.o \
			 bin/smalloc.o \

all: static

static: $(OBJECT_FILES)
	$ $(AR) -rcs $(LIBRARY_NAME).a $(OBJECT_FILES)

bin/%.o: src/%.c
	@echo "[+] Compiling $(notdir $<)"
	@ $(CC) -o $@ -c $< $(CFLAGS) $(ADDITIONAL_FLAGS)

test:
	echo "TESTING"

benchmark:
	echo "BENCHMARKING"

clean:
	@ rm bin/*.o

help:
	@echo "Targets:"
	@echo " · static: build a static library"
	@echo " · shared: build a shared library"
	@echo " · test: build a debugeable static library and test"
	@echo " · benchmark: build an optimized library and execute the benchmarks"
	@echo ""
	@echo "ADDITIONAL_FLAGS:"
	@echo " · SLAB_RELEASE: disable debugging prints"
	@echo " · SLAB_DEBUG: enable debugging prints"
	@echo " · SLAB_CONFIG_PARANOID_ASSERTS: enable paranoid asserts, testing stuff that should work but maybe doesn't"
	@echo " · POOL_CONFIG_PARANOID_ASSERTS: enable paranoid asserts, testing stuff that should work but maybe doesn't"
