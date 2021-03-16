#pragma once

#include <libio/Write.h>

namespace IO
{

struct Sink :
    public Writer
{
    ResultOr<size_t> write(const void *buffer, size_t size) override
    {
        __unused(buffer);
        return size;
    }
};

} // namespace IO
