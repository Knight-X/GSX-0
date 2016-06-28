#include "fio.h"
#include <string.h>
#include "filesystem.h"
#include "romfs.h"
#include "hash-djb2.h"

struct romfs_fds_t {
  const uint8_t *file;
  uint32_t cursor;
};


static struct romfs_fds_t romfs_fds[MAX_FDS];


static uint32_t get_unaligned(const uint8_t *meta) {
	return ((uint32_t)meta[0]) | ((uint32_t) (meta[1] << 8)) | ((uint32_t) (meta[2] << 16)) | ((uint32_t) (meta[3] << 24));
}
static ssize_t romfs_read(void *opaque, void *buf, size_t count) 
{
	struct romfs_fds_t *f = (struct romfs_fds_t *)opaque;
	const uint8_t *size_p = f->file - 4;
	uint32_t size = get_unaligned(size_p);

	if ((f->cursor + count) > size) 
		count = size - f->cursor;

	memcpy(buf, f->file + f->cursor, count);
	f->cursor += count;
	return count;

}

const uint8_t *get_file_from_hash(const uint8_t * romfs, uint32_t hash, uint32_t *len) 
{
	const uint8_t *meta;
	for (meta = romfs; get_unaligned(meta) && get_unaligned(meta + 4); meta += get_unaligned(meta + 4) + 8) {
		if (hash == get_unaligned(meta)) {
			if (len) {
				*len = get_unaligned(meta + 4);
			}
			return meta + 8;
		}
	}		
	return NULL;
}
int romfs_open(void *opaque, const char *path, int flags, int mode)
{
	uint32_t h = hash_djb2((const uint8_t *)path, -1);
	const uint8_t * romfs = (const uint8_t *) opaque;
	const uint8_t * file = get_file_from_hash(romfs, h, NULL);
	int r = -1;
	if (file) {
		r = fio_open(romfs_read, NULL, NULL, NULL, NULL);
		if (r > 0) {	
			romfs_fds[r].file = file;
			romfs_fds[r].cursor = 0;
			fio_set_opaque(r, romfs_fds + r);
		}
	}		
	return r;
}
	

void register_romfs(const char *mountpoint, void *opaque)
{
	register_fs(mountpoint, romfs_open, opaque);
}
