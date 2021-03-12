#pragma once

#include <string.h>

#include <libio/Writer.h>

namespace IO
{

static inline ResultOr<size_t> write(Writer &writer, char value)
{
    return writer.write(&value, sizeof(value));
}

static inline ResultOr<size_t> write(Writer &writer, const char *cstring)
{
    return writer.write(cstring, strlen(cstring));
}

} // namespace IO
