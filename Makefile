CC=gcc
AR=ar
CFLAGS+=-march=native -O2 -g

LIBRARY_NAME=libslaballocator

OBJECT_FILES=bin/slab_pool.o \
             bin/slab.o \
			 bin/smalloc.o \

all: static

static: $(OBJECT_FILES)
	$ $(AR) -rcs $(LIBRARY_NAME).a $(OBJECT_FILES)

bin/%.o: src/%.c
	@echo "[+] Compiling $(notdir $<)"
	@ $(CC) -o $@ -c $< $(CFLAGS)

test:
	echo "TESTING"

benchmark:
	echo "BENCHMARKING"

clean:
	@ rm bin/*.o