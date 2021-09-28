#pragma once

#include <libio/Seek.h>

namespace IO
{

struct Writer
{
    virtual ~Writer() {}

    virtual ResultOr<size_t> write(const void *buffer, size_t size) = 0;

    virtual HjResult flush() { return SUCCESS; }
};

template <typename T>
concept SeekableWriter = IsBaseOf<Writer, T>::value &&IsBaseOf<Seek, T>::value;

} // namespace IO