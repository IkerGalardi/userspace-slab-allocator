#ifndef MALLOC_INTEGRATION_H
#define MALLOC_INTEGRATION_H

typedef void*(*malloc_function)(size_t);
typedef void(*free_function)(void*);

extern malloc_function system_malloc;
extern free_function system_free;

#endif // MALLOC_INTEGRATION_H