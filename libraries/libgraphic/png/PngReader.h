#pragma once
#include <libgraphic/Color.h>
#include <libgraphic/png/PngCommon.h>
#include <libio/MemoryWriter.h>
#include <libio/Reader.h>
#include <libutils/Vector.h>

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

    inline uint32_t width()
    {
        return _width;
    }

    inline uint32_t height()
    {
        return _height;
    }

    inline const Vector<Color> &pixels()
    {
        return _pixels;
    }

private:
    Result uncompress(IO::MemoryWriter &uncompressed_writer);
    Result convert(Png::ColourType type, uint8_t *data);
    Result read();

    bool _valid = false;
    uint32_t _width = 0;
    uint32_t _height = 0;
    Vector<Color> _pixels;
    IO::Reader &_reader;
    IO::MemoryWriter _idat_writer;
};
} // namespace Graphic