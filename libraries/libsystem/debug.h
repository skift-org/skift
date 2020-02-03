#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/io/Stream.h>

void debug_hexdump(Stream *stream, char *desc, const void *addr, size_t len);