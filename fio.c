#include "fio.h"
#include "filesystem.h"
#include "romfs.h"
#include <string.h>

static struct fio_def_t fio_t[MAX_FS];


__attribute__((constructor)) void fio_init()
{
	memset(fio_t, 0, sizeof(fio_t));
}

static int fio_findfd()
{
	int i;
	for (i = 0; i < MAX_FDS; i++) {
		if (!fio_is_open(i)) {
			return i;
		}
	}
	return -1;
}

static int fio_is_open_s(int fd) {
	if (fd < 0 || (fd >= MAX_FDS))
		return 0;
	int r = !((fio_t[fd].fdread == NULL) &&
		  (fio_t[fd].fdwrite == NULL) &&
		  (fio_t[fd].fdseek == NULL) &&
		  (fio_t[fd].fdclose == NULL) &&
		  (fio_t[fd].opaque == NULL));
	return r;
}

int fio_is_open(int fd) {
	int r = 0;
	r = fio_is_open_s(fd);	
	return r;
}


int fio_open(fdread_t read_f, fdwrite_t write_f, fdseek_t seek_f, fdclose_t close_f, void *opaque)
{
	int fd;
	fd = fio_findfd();

	if (fd > 0) {
		fio_t[fd].fdread = read_f;
		fio_t[fd].fdwrite = write_f;
		fio_t[fd].fdseek = seek_f;
		fio_t[fd].fdclose = close_f;
		fio_t[fd].opaque = opaque;
	}

	return fd;
}

void fio_set_opaque(int fd, void *opaque) 
{
	if (fio_is_open(fd)) {
		fio_t[fd].opaque = opaque;
	}
}	
int fio_read(int fd, void *buf, size_t count) 
{
	int r;
	if (fio_is_open(fd)) {
		if (fio_t[fd].fdread) {
			r = fio_t[fd].fdread(fio_t[fd].opaque, buf, count);
		} else {
			r = -2;
		}
		r = -3;
	}
	return r;
}	

int fio_write(int fd, void *buf, size_t count)
{
	int r;
	if (fio_is_open(fd)) {
		if (fio_t[fd].fdread) {
			r = fio_t[fd].fdwrite(fio_t[fd].opaque, buf, count);
		} else {
			r = -2;
		}
		r = -3;
	}

	return r;
}	
