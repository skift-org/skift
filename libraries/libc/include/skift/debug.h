#pragma once

#include <skift/iostream.h>

void debug_hexdump(iostream_t *stream, char *desc, const void *addr, int len);