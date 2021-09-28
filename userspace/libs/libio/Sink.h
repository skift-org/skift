#pragma once

#include <libio/Write.h>

namespace IO
{

struct Sink :
    public Writer
{
    ResultOr<size_t> write(const void *buffer, size_t size) override
    {
        UNUSED(buffer);
        return size;
    }
};

} // namespace IO
