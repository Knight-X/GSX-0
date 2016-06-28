#ifndef    __ROMFS_H__
#define    __ROMFS_H__

#include <stdint.h>

void register_romfs(const char *mountpoint, void *opaque);
const uint8_t *get_file_from_hash(const uint8_t *romfs, uint32_t h, uint32_t *len);

#endif
