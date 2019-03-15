#pragma once

#include <skift/generic.h>
#include "kernel/shared/filesystem.h"

int  sk_filesystem_open(const char *path, fsoflags_t flags);
void sk_filesystem_close(int fd);

int sk_filesystem_read (int fd, void *buffer, uint size);
int sk_filesystem_write(int fd, void *buffer, uint size);

int sk_filesystem_seek(int fd, int offset, seek_origin_t origin);
int sk_filesystem_tell(int fd);

int sk_filesystem_fstat(int fd, file_stat_t *stat);

int sk_filesystem_mkdir(const char *path);
int sk_filesystem_unlink(const char *path);