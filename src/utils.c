#include "utils.h"

#include <stdint.h>

inline bool is_ptr_in_page(const void* page, const void* ptr) {
    const void* page_end = (const void*)((const uint8_t*)page + SLAB_PAGE_SIZE);
    return (ptr > page) && (page_end > ptr);
}

inline void* get_page_pointer(void* ptr) {
    // NOTE: assumes 4k pages. maybe some way to detect 16k pages?
    return (void*)((uintptr_t)ptr & (~0xFFF));
}
