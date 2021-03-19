#pragma once

#include <libio/Copy.h>
#include <libio/Sink.h>

namespace IO
{

static inline Result skip(Reader &from, size_t n)
{
    Sink sink;
    return copy(from, sink, n);
}

} // namespace IO