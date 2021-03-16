#pragma once

#include <libio/Reader.h>

namespace IO
{

struct Repeat :
    public Reader
{
private:
    uint8_t _c;

public:
    Repeat(uint8_t c) : _c{c} {}

    ResultOr<size_t> read(void *buffer, size_t size)
    {
        for (size_t i = 0; i < size; i++)
        {
            ((uint8_t *)buffer)[i] = _c;
        }

        return size;
    }
};

} // namespace IO