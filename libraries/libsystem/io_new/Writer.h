#pragma once

#include <libutils/ResultOr.h>
#include <libutils/unicode/Codepoint.h>

#include <libsystem/io_new/Seek.h>

namespace System
{

class Writer
{
public:
    virtual ~Writer() { flush(); }

    virtual ResultOr<size_t> write(const void *buffer, size_t size) = 0;

    virtual ResultOr<size_t> write(char v) { return write(&v, 1); }

    virtual ResultOr<size_t> write(uint8_t v) { return write(&v, 1); }

    ResultOr<size_t> write(const char *buffer) { return write(buffer, strlen(buffer)); }

    virtual Result flush() { return SUCCESS; }

    ResultOr<size_t> write(Codepoint cp)
    {
        char buffer[5];
        codepoint_to_utf8(cp, (uint8_t *)buffer);
        return write(buffer);
    }
};

template <typename T>
concept SeekableWriter = IsBaseOf<Writer, T>::value &&IsBaseOf<Seek, T>::value;

} // namespace System