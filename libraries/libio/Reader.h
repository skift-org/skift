#pragma once

#include <libutils/ResultOr.h>

#include <libio/Seek.h>

namespace IO
{

class Reader
{
public:
    virtual ResultOr<size_t> read(void *buffer, size_t size) = 0;

    virtual ResultOr<uint8_t> read_byte()
    {
        uint8_t byte = 0xfe;
        TRY(read(&byte, 1));
        return byte;
    };
};

template <typename T>
concept SeekableReader = IsBaseOf<Reader, T>::value &&IsBaseOf<Seek, T>::value;

} // namespace IO