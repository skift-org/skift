#include <libcompression/Inflate.h>
#include <libio/CRCReader.h>
#include <libio/Read.h>
#include <libio/ScopedReader.h>
#include <libio/Skip.h>
#include <libio/Streams.h>
#include <libpng/Reader.h>
#include <libutils/Array.h>
#include <libutils/Assert.h>
#include <libutils/Chrono.h>

namespace Png
{

Reader::Reader(IO::Reader &reader) : _reader(reader)
{
    _valid = read() == HjResult::SUCCESS;
}

HjResult Reader::read()
{
    Array<uint8_t, 8> signature;
    _reader.read(signature.raw_storage(), sizeof(signature));

    // Doesn't matter if the read above can't read all bytes, this will fail anyways
    if (signature != Array<uint8_t, 8>{137, 80, 78, 71, 13, 10, 26, 10})
    {
        IO::logln("Invalid PNG signature!");
        return ERR_INVALID_DATA;
    }

    TRY(read_chunks());

    IO::MemoryWriter uncompressed_writer;
    TRY(uncompress(uncompressed_writer));
    uint8_t *scanlines = uncompressed_writer.buffer();

    // Unfilter the scanlines
    size_t out_size = _width * _height * num_channels() * bytes_per_pixel();
    uint8_t *raw_buffer = new uint8_t[out_size];
    memset(raw_buffer, 0, out_size);
    TRY(unfilter(raw_buffer, scanlines));

    TRY(convert(raw_buffer));
    delete[] raw_buffer;

    return HjResult::SUCCESS;
}

HjResult Reader::read_chunks()
{
    size_t idat_counter = 0;
    bool end = false;
    uint32_t prev_signature = 0;
    while (!end)
    {
        auto chunk_length = TRY(IO::read<be_uint32_t>(_reader));

        // CRC checksum includes the chunk signature and chunk data
        // See https://www.w3.org/TR/2003/REC-PNG-20031110/#5Introduction
        IO::CRCReader crc_reader(_reader);
        auto chunk_signature = TRY(IO::read<be_uint32_t>(crc_reader));
        IO::ScopedReader scoped_reader(crc_reader, chunk_length());

        switch (chunk_signature())
        {
        case ImageHeader::SIG:
        {
            auto image_header = TRY(IO::read<ImageHeader>(scoped_reader));
            _width = image_header.width();
            _height = image_header.height();

            // We don't support ADAM7 yet
            if (image_header.interlace_method() != 0)
            {
                IO::logln("Unsupported interlace method: {}", image_header.interlace_method());
                return ERR_NOT_IMPLEMENTED;
            }

            // Same for bit depth
            if (image_header.bit_depth() != 8)
            {
                IO::logln("Unsupported bitdepth: {}", image_header.bit_depth());
                return ERR_NOT_IMPLEMENTED;
            }

            _bit_depth = image_header.bit_depth();
            _colour_type = image_header.colour_type();
        }
        break;

        case Gamma::SIG:
        {
            TRY(IO::read<Gamma>(scoped_reader));
        }
        break;

        case Chroma::SIG:
        {
            TRY(IO::read<Chroma>(scoped_reader));
        }
        break;

        case BackgroundColor::SIG:
        {
            Vector<uint8_t> data;
            TRY(IO::read_vector(scoped_reader, data));
        }
        break;

        case Time::SIG:
        {
            auto modified_date = TRY(IO::read<Time>(scoped_reader));
            _modified.year = modified_date.year();
            _modified.month = modified_date.month();
            _modified.day = modified_date.day();
            _modified.hour = modified_date.hour();
            _modified.minute = modified_date.minute();
            _modified.second = modified_date.second();
        }
        break;

        case sRGB::SIG:
        {
            TRY(IO::read<sRGB>(scoped_reader));
        }
        break;

        case Palette::SIG:
        {
            auto num_entries = chunk_length() / 3;
            for (size_t i = 0; i < num_entries; i++)
            {
                uint8_t red = TRY(IO::read<uint8_t>(scoped_reader));
                uint8_t green = TRY(IO::read<uint8_t>(scoped_reader));
                uint8_t blue = TRY(IO::read<uint8_t>(scoped_reader));

                _palette.push_back(Graphic::Color::from_rgb_byte(red, green, blue));
            }
        }
        break;

        case Transparency::SIG:
        {
            // Must appear after palette chunk according to specification
            // See: https://www.w3.org/TR/2003/REC-PNG-20031110/#5ChunkOrdering
            if (_colour_type == CT_PALETTE)
            {
                auto num_entries = chunk_length();
                if (num_entries > _palette.count())
                {
                    IO::logln("Transparency chunk has more entries than current palette");
                    return ERR_INVALID_DATA;
                }

                for (size_t i = 0; i < num_entries; i++)
                {
                    uint8_t alpha = TRY(IO::read<uint8_t>(scoped_reader));
                    _palette[i] = _palette[i].with_alpha_byte(alpha);
                }
            }
            else if (_colour_type == CT_GREY || _colour_type == CT_RGB)
            {
                IO::logln("Transparency chunk not implemented for current colour type");
                return ERR_NOT_IMPLEMENTED;
            }
            else
            {
                IO::logln("Transparency chunk not allowed for current colour type");
                return ERR_INVALID_DATA;
            }
        }
        break;

        case ImageData::SIG:
        {
            // The first chunk begins with the compression method
            if (idat_counter > 0 && prev_signature != ImageData::SIG)
            {
                IO::logln("Multiple iDat chunks must be subsequent");
                return ERR_INVALID_DATA;
            }

            // Read the data for this chunk into our concatenated compressed data
            TRY(IO::copy(scoped_reader, _idat_writer));
            idat_counter++;
        }
        break;

        case TextualData::SIG:
        {
            Vector<uint8_t> data;
            TRY(IO::read_vector(scoped_reader, data));
        }
        break;

        case PhysicalDimensions::SIG:
            TRY(IO::read<PhysicalDimensions>(scoped_reader));
            break;

        case ImageEnd::SIG:
        {
            end = true;
        }
        break;

        default:
        {
            IO::logln("Unknown PNG chunk: {08x}", chunk_signature());
            Vector<uint8_t> data;
            TRY(IO::read_vector(scoped_reader, data));
        }
        break;
        }

        auto crc = TRY(IO::read<be_uint32_t>(_reader));
        if (crc() != crc_reader.checksum())
        {
            IO::logln("Chunk checksum validation failed");
            return ERR_INVALID_DATA;
        }
        prev_signature = chunk_signature();
    }

    return HjResult::SUCCESS;
}

// Copyright (c) 2005-2020 Lode Vandevenne
HjResult Reader::unfilter(uint8_t *out, uint8_t *in)
{
    // For PNG filter method 0
    // this function unfilters a single image (e.g. without interlacing this is called once, with Adam7 seven times)
    // out must have enough bytes allocated already, in must have the scanlines + 1 filter_type byte per scanline
    // w and h are image dimensions or dimensions of reduced image, bpp is bits per pixel
    // in and out are allowed to be the same memory address (but aren't the same size since in has the extra filter bytes)
    uint8_t *prevline = 0;

    // bytewidth is used for filtering, is 1 when bpp < 8, number of bytes per pixel otherwise
    size_t bytewidth = (bits_per_pixel() + 7u) / 8u;
    size_t linebytes = (_width * bits_per_pixel() + 7u) / 8u;

    for (uint32_t y = 0; y < _height; ++y)
    {
        size_t outindex = linebytes * y;
        size_t inindex = (1 + linebytes) * y; /*the extra filterbyte added to each row*/
        FilterType filter_type = (FilterType)in[inindex];

        TRY(unfilter_scanline(&out[outindex], &in[inindex + 1], prevline, bytewidth, filter_type, linebytes));

        prevline = &out[outindex];
    }

    return HjResult::SUCCESS;
}

// Path predictor, used by PNG filter type 4
// The parameters are of type short, but should come from unsigned charunsigned chars, the shorts
// are only needed to make the paeth calculation correct.
static uint8_t paeth_predictor(int16_t a, int16_t b, int16_t c)
{
    int16_t pa = abs(b - c);
    int16_t pb = abs(a - c);
    int16_t pc = abs(a + b - c - c);

    // Return input value associated with smallest of pa, pb, pc (with certain priority if equal)
    if (pb < pa)
    {
        a = b;
        pa = pb;
    }

    return (pc < pa) ? c : a;
}

// Copyright (c) 2005-2020 Lode Vandevenne
HjResult Reader::unfilter_scanline(uint8_t *recon, const uint8_t *scanline, const uint8_t *precon,
                                   size_t bytewidth, FilterType filter_type, size_t length)
{
    // For PNG filter method 0
    // unfilter a PNG image scanline by scanline. when the pixels are smaller than 1 byte,
    // the filter works byte per byte (bytewidth = 1)
    // precon is the previous unfiltered scanline, recon the result, scanline the current one
    // the incoming scanlines do NOT include the filter_type byte, that one is given in the parameter filter_type instead
    // recon and scanline MAY be the same memory address! precon must be disjoint.
    size_t i;

    switch (filter_type)
    {
    case FT_NONE:
        for (i = 0; i != length; ++i)
        {
            recon[i] = scanline[i];
        }
        break;

    case FT_SUB:
        for (i = 0; i != bytewidth; ++i)
        {
            recon[i] = scanline[i];
        }

        for (i = bytewidth; i < length; ++i)
        {
            recon[i] = scanline[i] + recon[i - bytewidth];
        }
        break;

    case FT_UP:
        if (precon)
        {
            for (i = 0; i != length; ++i)
            {
                recon[i] = scanline[i] + precon[i];
            }
        }
        else
        {
            for (i = 0; i != length; ++i)
            {
                recon[i] = scanline[i];
            }
        }
        break;

    case FT_AVERAGE:
        if (precon)
        {
            for (i = 0; i != bytewidth; ++i)
            {
                recon[i] = scanline[i] + (precon[i] >> 1u);
            }

            for (i = bytewidth; i < length; ++i)
            {
                recon[i] = scanline[i] + ((recon[i - bytewidth] + precon[i]) >> 1u);
            }
        }
        else
        {
            for (i = 0; i != bytewidth; ++i)
            {
                recon[i] = scanline[i];
            }

            for (i = bytewidth; i < length; ++i)
            {
                recon[i] = scanline[i] + (recon[i - bytewidth] >> 1u);
            }
        }
        break;

    case FT_PAETH:
        if (precon)
        {
            for (i = 0; i != bytewidth; ++i)
            {
                // paeth_predictor(0, precon[i], 0) is always precon[i]
                recon[i] = (scanline[i] + precon[i]);
            }

            if (bytewidth >= 2)
            {
                for (; i + 1 < length; i += 2)
                {
                    size_t j = i - bytewidth;
                    uint8_t s0 = scanline[i + 0], s1 = scanline[i + 1];
                    uint8_t r0 = recon[j + 0], r1 = recon[j + 1];
                    uint8_t p0 = precon[i + 0], p1 = precon[i + 1];
                    uint8_t q0 = precon[j + 0], q1 = precon[j + 1];
                    recon[i + 0] = s0 + paeth_predictor(r0, p0, q0);
                    recon[i + 1] = s1 + paeth_predictor(r1, p1, q1);
                }
            }

            for (; i != length; ++i)
            {
                recon[i] = (scanline[i] + paeth_predictor(recon[i - bytewidth], precon[i], precon[i - bytewidth]));
            }
        }
        else
        {
            for (i = 0; i != bytewidth; ++i)
            {
                recon[i] = scanline[i];
            }

            for (i = bytewidth; i < length; ++i)
            {
                // paeth_predictor(recon[i - bytewidth], 0, 0) is always recon[i - bytewidth]
                recon[i] = (scanline[i] + recon[i - bytewidth]);
            }
        }
        break;

    default:
        IO::logln("Invalid filter type: {}", filter_type);
        // error: nonexistent filter type given
        return ERR_INVALID_DATA;
    }

    return HjResult::SUCCESS;
}

HjResult Reader::uncompress(IO::MemoryWriter &uncompressed_writer)
{
    IO::MemoryReader compressed_reader(Slice(_idat_writer.slice()));

    // Two bytes before the actual deflate data
    // See https://www.w3.org/TR/2003/REC-PNG-20031110/#10Compression
    auto cm_cinfo = TRY(IO::read<uint8_t>(compressed_reader));

    // ZLib compression mode should be DEFLATE
    if ((cm_cinfo & 15) != 8)
    {
        IO::logln("Invalid zlib compression mode for PNG");
        return ERR_INVALID_DATA;
    }

    // Sliding window should be 32k at max
    if (((cm_cinfo >> 4) & 15) > 7)
    {
        IO::logln("Invalid zlib sliding window size for PNG");
        return ERR_INVALID_DATA;
    }

    auto flags = TRY(IO::read<uint8_t>(compressed_reader));
    UNUSED(flags);

    // Decode our compressed image data
    Compression::Inflate inflate;
    return inflate.perform(compressed_reader, uncompressed_writer).result();
}

HjResult Reader::convert(uint8_t *buffer)
{
    _pixels.resize(_width * _height);

    switch (_colour_type)
    {
    case CT_RGBA:
        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = Graphic::Color::from_rgba_byte(buffer[i * 4],
                                                        buffer[i * 4 + 1],
                                                        buffer[i * 4 + 2],
                                                        buffer[i * 4 + 3]);
        }
        break;
    case CT_RGB:
        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = Graphic::Color::from_rgb_byte(buffer[i * 3],
                                                       buffer[i * 3 + 1],
                                                       buffer[i * 3 + 2]);
        }
        break;

    case CT_GREY:
        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = Graphic::Color::from_monochrome_byte(buffer[i]);
        }
        break;
    case CT_GREY_ALPHA:
        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = Graphic::Color::from_monochrome_alpha_byte(buffer[i * 2],
                                                                    buffer[i * 2 + 1]);
        }
        break;
    case CT_PALETTE:
        if (_palette.empty())
        {
            IO::logln("Palette colour type requires a palett data");
            return ERR_INVALID_DATA;
        }

        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = _palette[buffer[i]];
        }
        break;
    default:
        IO::logln("Unsupported PNG colour type: %u", _colour_type);
        return ERR_NOT_IMPLEMENTED;
    }

    return HjResult::SUCCESS;
}

} // namespace Png
