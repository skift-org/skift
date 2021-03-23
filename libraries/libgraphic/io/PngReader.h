#pragma once
#include <libio/Reader.h>

namespace Graphic
{
class PngReader
{
public:
    PngReader(IO::Reader &reader);

    inline bool valid()
    {
        return _valid;
    }

private:
    Result read();

    bool _valid = false;
    uint32_t _width = 0;
    uint32_t _height = 0;
    IO::Reader &_reader;
};
} // namespace Graphic