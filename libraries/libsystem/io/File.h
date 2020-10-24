#pragma once

#include <libsystem/Common.h>
#include <libsystem/Result.h>

Result file_read_all(const char *path, void **buffer, size_t *size);

Result file_write_all(const char *path, void *buffer, size_t size);

bool file_exist(const char *path);

Result file_copy(const char *src, const char *dst);
