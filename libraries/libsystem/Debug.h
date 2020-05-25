#pragma once

#include <libsystem/io/Stream.h>

void debug_hexdump(Stream *stream, char *desc, const void *addr, size_t len);
