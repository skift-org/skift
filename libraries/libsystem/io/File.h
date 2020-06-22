#pragma once

#include <libsystem/Common.h>
#include <libsystem/Result.h>

Result file_read_all(const char *path, void **buffer, size_t *size);
