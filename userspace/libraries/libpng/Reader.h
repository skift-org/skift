#pragma once

#include <abi/Time.h>

#include <libgraphic/Color.h>
#include <libio/MemoryWriter.h>
#include <libio/Reader.h>
#include <libpng/Common.h>
#include <libutils/Vector.h>

namespace Png
{

struct Reader
{
private:
    bool _valid = false;
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint8_t _bit_depth = 0;
    Vector<Graphic::Color> _pixels;
    Vector<Graphic::Color> _palette;
    DateTime _modified;
    ColourType _colour_type;
    IO::Reader &_reader;
    IO::MemoryWriter _idat_writer;

    HjResult uncompress(IO::MemoryWriter &uncompressed_writer);
    HjResult unfilter(uint8_t *in, uint8_t *out);
    HjResult unfilter_scanline(uint8_t *recon, const uint8_t *scanline, const uint8_t *precon,
                               size_t bytewidth, Png::FilterType filterType, size_t length);
    HjResult convert(uint8_t *data);
    HjResult read_chunks();

    HjResult read();

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
    inline const Vector<Graphic::Color> &pixels() const { return _pixels; }
    inline const DateTime &modified() const { return _modified; }

    Reader(IO::Reader &reader);
};

} // namespace Png