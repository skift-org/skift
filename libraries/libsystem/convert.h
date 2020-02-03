#pragma once

/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libsystem/runtime.h>

uint convert_string_to_uint(const char *str, int n, int base);

int convert_uint_to_string(uint value, char *str, int n, int base);