#pragma once

#include <abi/Time.h>

#include <libgraphic/Color.h>
#include <libgraphic/png/PngCommon.h>
#include <libio/MemoryWriter.h>
#include <libio/Reader.h>
#include <libutils/Vector.h>

namespace Graphic
{
class PngReader
{
private:
    bool _valid = false;
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint8_t _bit_depth = 0;
    Vector<Color> _pixels;
    Vector<Color> _palette;
    DateTime _modified;
    Png::ColourType _colour_type;
    IO::Reader &_reader;
    IO::MemoryWriter _idat_writer;

    Result uncompress(IO::MemoryWriter &uncompressed_writer);
    Result unfilter(uint8_t *in, uint8_t *out);
    Result unfilter_scanline(uint8_t *recon, const uint8_t *scanline, const uint8_t *precon,
                             size_t bytewidth, Png::FilterType filterType, size_t length);
    Result convert(uint8_t *data);
    Result read_chunks();

    Result read();

    int num_channels()
    {
        switch (_colour_type)
        {
        case Png::CT_GREY:
            return 1;
        case Png::CT_RGB:
            return 3;
        case Png::CT_PALETTE:
            return 1;
        case Png::CT_GREY_ALPHA:
            return 2;
        case Png::CT_RGBA:
            return 4;
        default:
            return 0; /*invalid color type*/
        }
    }

    inline size_t bytes_per_pixel()
    {
        return (bits_per_pixel()) / 8;
    }

    inline size_t bits_per_pixel()
    {
        return (num_channels() * _bit_depth);
    }

public:
    inline bool valid() const { return _valid; }
    inline uint32_t width() const { return _width; }
    inline uint32_t height() const { return _height; }
    inline const Vector<Color> &pixels() const { return _pixels; }
    inline const DateTime &modified() const { return _modified; }

    PngReader(IO::Reader &reader);
};

} // namespace Graphic