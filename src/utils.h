#ifndef SLAB_UTILS_H
#define SLAB_UTILS_H

#include <stdbool.h>
#include <unistd.h>

#define NON_EXISTANT (uint16_t)(-1)

#define SLAB_PAGE_SIZE sysconf(_SC_PAGESIZE)

#define MAYBE_UNUSED __attribute__((unused))

#define UNUSED_PARAMETER(x) (void)(x)

/*
 * Returns true if the pointer is allocated on the page.
 *
 * @param page: pointer to the start of the page
 * @param ptr: pointer to be tested
 * @return true if the pointer belongs in the page, false if not.
 */
bool is_ptr_in_page(const void* page, const void* ptr);

/*
 * Returns the pointer to the start of the page given a pointer.
 *
 * @param ptr: pointer to find the start of the page
 * @return: pointer to the start of the page of 'ptr'
 */
void* get_page_pointer(void* ptr);

#endif // SLAB_UTILS_H
