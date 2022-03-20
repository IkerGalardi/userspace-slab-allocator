#ifndef SMALLOC_H
#define SMALLOC_H

#include <stddef.h>

/*
 * Initializes all the slabs.
 */
void smalloc_initialize();

/*
 * Allocates memory trying to use slab caches as much as possible.
 *
 * @param size: size of the allocation
 */
void* smalloc(size_t size);

/* 
 * Free's a previously allocated pointer.
 *
 * @param ptr: pointer to free 
 */
void sfree(void* ptr);

#endif // SMALLOC_H
