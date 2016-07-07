#include "fio.h"
#include "filesystem.h"
#include "romfs.h"
#include <string.h>
#include "os.h"

static struct fio_def_t fio_t[MAX_FS];

static ssize_t stdin_read(void *opaque, void *buf, size_t count) {
	return 0;
}

static ssize_t stdin_write(void *opaque, void *buf, size_t count) {
	const char *data = (const char *)buf;
		print_str(data);

	return count;
}

__attribute__((constructor)) void fio_init()
{
	memset(fio_t, 0, sizeof(fio_t));
	fio_t[0].fdread = stdin_read;
	fio_t[1].fdwrite = stdin_write;
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
int fio_read(int fd, void *buf, size_t count, tcb_t *task_t) 
{
	tcb_t *task = task_t;
	int r;
	if (fio_is_open(fd)) {
		if (fio_t[fd].fdread) {
			r = fio_t[fd].fdread(fio_t[fd].opaque, buf, count);
			((uint32_t *)task->stack)[9] = r;
		} else {
			r = -3;
		}
	} else {
		r = -2;
	}
	return r;
}	

int fio_write(int fd, void *buf, size_t count)
{
	int r;
	if (fio_is_open(fd)) {
		if (fio_t[fd].fdwrite) {
			r = fio_t[fd].fdwrite(fio_t[fd].opaque, buf, count);
		} else {
			r = -3;
		}
	} else {
		return -2;
	}

	return r;
}	
