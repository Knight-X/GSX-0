#ifndef  __FILESYSTEM_H__
#define  __FILESYSTEM_H__


#include <stdint.h>
#include "hash-djb2.h"

#define MAX_FS 16

typedef int (*fs_open_t)(void *opaque, const char *fname, int flags, int mode);

__attribute__((constructor)) void fs_init();

int register_fs(const char *mountpoint, fs_open_t callback, void *opaque);
int open(const char *path, int flags, int mode);

#endif 
