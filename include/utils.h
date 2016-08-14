#ifndef __UTILS_H_
#define __UTILS_H_

#include <stddef.h>


#define container_of(ptr, type, member) \
    ((type *)(((void *)ptr) - offsetof(type, member)))

#endif
