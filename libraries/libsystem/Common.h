#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <abi/Process.h>
#include <libsystem/Macros.h>
#include <libsystem/core/Allocator.h>

struct __SOURCE_LOCATION__
{
    const char *file;
    const char *function;
    int line;
};

#define SOURCE_LOCATION \
    (__SOURCE_LOCATION__{__FILE__, __FUNCTION__, __LINE__})

#define INVALID_SOURCE_LOCATION \
    (__SOURCE_LOCATION__{"no-file", "no-function", 69})
