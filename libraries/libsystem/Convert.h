#pragma once

#include <libsystem/Common.h>

uint convert_string_to_uint(const char *str, int n, int base);

int convert_uint_to_string(uint value, char *str, int n, int base);
