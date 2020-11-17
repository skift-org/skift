#pragma once

#include <libutils/ResultOr.h>
#include <libutils/Slice.h>
#include <libutils/String.h>

#include <libsystem/Common.h>
#include <libsystem/Result.h>

Result file_read_all(const char *path, void **buffer, size_t *size);

ResultOr<Slice> file_read_all(String path);

Result file_write_all(const char *path, void *buffer, size_t size);

bool file_exist(const char *path);

Result file_copy(const char *src, const char *dst);
