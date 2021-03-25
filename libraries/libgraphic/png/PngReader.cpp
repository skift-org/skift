#include <libcompression/Inflate.h>
#include <libgraphic/png/PngReader.h>
#include <libio/Read.h>
#include <libio/ScopedReader.h>
#include <libio/Skip.h>
#include <libsystem/Logger.h>
#include <libtest/Asserts.h>
#include <libutils/Array.h>

Graphic::PngReader::PngReader(IO::Reader &reader) : _reader(reader)
{
    _valid = read() == Result::SUCCESS;
}

Result Graphic::PngReader::read()
{
    Array<uint8_t, 8> signature;
    _reader.read(signature.raw_storage(), sizeof(signature));

    // Doesn't matter if the read above can't read all bytes, this will fail anyways
    if (signature != Array<uint8_t, 8>{137, 80, 78, 71, 13, 10, 26, 10})
    {
        logger_error("Invalid PNG signature!");
        return Result::ERR_INVALID_DATA;
    }

    TRY(read_chunks());

    // Uncompress the scanlines
    IO::MemoryWriter uncompressed_writer;
    TRY(uncompress(uncompressed_writer));
    uint8_t *scanlines = uncompressed_writer.buffer();
    // Unfilter the scanlines
    size_t out_size = _width * _height * num_channels() * bytes_per_pixel();
    uint8_t *raw_buffer = new uint8_t[out_size];
    memset(raw_buffer, 0, out_size);
    TRY(unfilter(raw_buffer, scanlines));
    // Convert to our target format
    TRY(convert(raw_buffer));
    delete[] raw_buffer;

    return Result::SUCCESS;
}

Result Graphic::PngReader::read_chunks()
{
    size_t idat_counter = 0;
    bool end = false;
    uint32_t prev_signature = 0;
    while (!end)
    {
        auto chunk_length = TRY(IO::read<be_uint32_t>(_reader));
        auto chunk_signature = TRY(IO::read<be_uint32_t>(_reader));

        switch (chunk_signature())
        {
        case Png::ImageHeader::SIG:
        {
            auto image_header = TRY(IO::read<Png::ImageHeader>(_reader));
            _width = image_header.width();
            _height = image_header.height();
            // We don't support ADAM7 yet
            if (image_header.interlace_method() != 0)
            {
                logger_error("Unsupported interlace method: %u", image_header.interlace_method());
                return Result::ERR_NOT_IMPLEMENTED;
            }
            // Same for bit depth
            if (image_header.bit_depth() != 8)
            {
                logger_error("Unsupported bitdepth: %u", image_header.bit_depth());
                return Result::ERR_NOT_IMPLEMENTED;
            }
            _bit_depth = image_header.bit_depth();
            _colour_type = image_header.colour_type();
        }
        break;

        case Png::Gamma::SIG:
        {
            TRY(IO::read<Png::Gamma>(_reader));
        }
        break;

        case Png::Chroma::SIG:
        {
            TRY(IO::read<Png::Chroma>(_reader));
        }
        break;

        case Png::BackgroundColor::SIG:
        {
            char buf[6];
            _reader.read(buf, chunk_length());
        }
        break;

        case Png::Time::SIG:
        {
            auto modified_date = TRY(IO::read<Png::Time>(_reader));
            _modified.year = modified_date.year();
            _modified.month = modified_date.month();
            _modified.day = modified_date.day();
            _modified.hour = modified_date.hour();
            _modified.minute = modified_date.minute();
            _modified.second = modified_date.second();
        }
        break;

        case Png::sRGB::SIG:
        {
            TRY(IO::read<Png::sRGB>(_reader));
        }
        break;

        case Png::Palette::SIG:
        {
            auto num_entries = chunk_length() / 3;
            for (size_t i = 0; i < num_entries; i++)
            {
                uint8_t red = TRY(IO::read<uint8_t>(_reader));
                uint8_t green = TRY(IO::read<uint8_t>(_reader));
                uint8_t blue = TRY(IO::read<uint8_t>(_reader));
                _palette.push_back(Color::from_rgb_byte(red, green, blue));
            }
        }
        break;

        case Png::Transparency::SIG:
        {
            // Must appear after palette chunk according to specification
            // See: https://www.w3.org/TR/2003/REC-PNG-20031110/#5ChunkOrdering
            if (_colour_type == Png::CT_PALETTE)
            {
                auto num_entries = chunk_length();
                if(num_entries > _palette.count())
                {
                    logger_error("Transparency chunk has more entries than current palette");
                    return Result::ERR_INVALID_DATA;
                }

                for (size_t i = 0; i < num_entries; i++)
                {
                    uint8_t alpha = TRY(IO::read<uint8_t>(_reader));
                    _palette[i] = _palette[i].with_alpha_byte(alpha);
                }
            }
            else if (_colour_type == Png::CT_GREY || _colour_type == Png::CT_RGB)
            {
                logger_error("Transparency chunk not implemented for current colour type");
                return Result::ERR_NOT_IMPLEMENTED;
            }
            else
            {
                logger_error("Transparency chunk not allowed for current colour type");
                return Result::ERR_INVALID_DATA;
            }
        }
        break;

        case Png::ImageData::SIG:
        {
            size_t data_size = chunk_length();

            // The first chunk begins with the compression method
            if (idat_counter == 0)
            {
                data_size -= 2;
                // Two bytes before the actual deflate data
                // See https://www.w3.org/TR/2003/REC-PNG-20031110/#10Compression
                auto cm_cinfo = TRY(IO::read<uint8_t>(_reader));
                // ZLib compression mode should be DEFLATE
                assert_equal(cm_cinfo & 15, 8);
                // Sliding window should be 32k at max
                assert_lower_equal((cm_cinfo >> 4) & 15, 7);
                auto flags = TRY(IO::read<uint8_t>(_reader));
                __unused(flags);
            }
            else
            {
                // Multiple iDat chunks must be subsequent
                assert_equal(prev_signature, Png::ImageData::SIG);
            }

            // Read the data for this chunk into our concatenated compressed data
            IO::ScopedReader data_reader(_reader, data_size);
            TRY(IO::copy(data_reader, _idat_writer));
            idat_counter++;
        }
        break;

        case Png::TextualData::SIG:
        {
            Vector<uint8_t> data(chunk_length());
            TRY(_reader.read(data.raw_storage(), chunk_length()));
        }
        break;

        case Png::PhysicalDimensions::SIG:
            TRY(IO::read<Png::PhysicalDimensions>(_reader));
            break;

        case Png::ImageEnd::SIG:
        {
            end = true;
        }
        break;

        default:
        {
            logger_error("Unknown PNG chunk: %u", chunk_signature());
            Vector<uint8_t> data(chunk_length());
            TRY(_reader.read(data.raw_storage(), chunk_length()));
        }
        break;
        }

        auto crc = TRY(IO::read<be_uint32_t>(_reader));
        __unused(crc);
        prev_signature = chunk_signature();
    }

    return Result::SUCCESS;
}

// Copyright (c) 2005-2020 Lode Vandevenne
Result Graphic::PngReader::unfilter(uint8_t *out, uint8_t *in)
{
    /*
  For PNG filter method 0
  this function unfilters a single image (e.g. without interlacing this is called once, with Adam7 seven times)
  out must have enough bytes allocated already, in must have the scanlines + 1 filtertype byte per scanline
  w and h are image dimensions or dimensions of reduced image, bpp is bits per pixel
  in and out are allowed to be the same memory address (but aren't the same size since in has the extra filter bytes)
  */
    uint8_t *prevline = 0;

    /*bytewidth is used for filtering, is 1 when bpp < 8, number of bytes per pixel otherwise*/
    size_t bytewidth = (bits_per_pixel() + 7u) / 8u;
    size_t linebytes = (_width * bits_per_pixel() + 7u) / 8u;

    for (uint32_t y = 0; y < _height; ++y)
    {
        size_t outindex = linebytes * y;
        size_t inindex = (1 + linebytes) * y; /*the extra filterbyte added to each row*/
        Png::FilterType filterType = (Png::FilterType)in[inindex];

        TRY(unfilterScanline(&out[outindex], &in[inindex + 1], prevline, bytewidth, filterType, linebytes));

        prevline = &out[outindex];
    }

    return Result::SUCCESS;
}

/*
Path predictor, used by PNG filter type 4
The parameters are of type short, but should come from unsigned charunsigned chars, the shorts
are only needed to make the paeth calculation correct.
*/
static uint8_t paethPredictor(int16_t a, int16_t b, int16_t c)
{
    int16_t pa = abs(b - c);
    int16_t pb = abs(a - c);
    int16_t pc = abs(a + b - c - c);
    /* return input value associated with smallest of pa, pb, pc (with certain priority if equal) */
    if (pb < pa)
    {
        a = b;
        pa = pb;
    }
    return (pc < pa) ? c : a;
}

// Copyright (c) 2005-2020 Lode Vandevenne
Result Graphic::PngReader::unfilterScanline(uint8_t *recon, const uint8_t *scanline, const uint8_t *precon,
                                            size_t bytewidth, Png::FilterType filterType, size_t length)
{
    /*
  For PNG filter method 0
  unfilter a PNG image scanline by scanline. when the pixels are smaller than 1 byte,
  the filter works byte per byte (bytewidth = 1)
  precon is the previous unfiltered scanline, recon the result, scanline the current one
  the incoming scanlines do NOT include the filtertype byte, that one is given in the parameter filterType instead
  recon and scanline MAY be the same memory address! precon must be disjoint.
  */
    size_t i;
    switch (filterType)
    {
    case Png::FT_None:
        for (i = 0; i != length; ++i)
            recon[i] = scanline[i];
        break;
    case Png::FT_Sub:
        for (i = 0; i != bytewidth; ++i)
            recon[i] = scanline[i];
        for (i = bytewidth; i < length; ++i)
            recon[i] = scanline[i] + recon[i - bytewidth];
        break;
    case Png::FT_Up:
        if (precon)
        {
            for (i = 0; i != length; ++i)
                recon[i] = scanline[i] + precon[i];
        }
        else
        {
            for (i = 0; i != length; ++i)
                recon[i] = scanline[i];
        }
        break;
    case Png::FT_Average:
        if (precon)
        {
            for (i = 0; i != bytewidth; ++i)
                recon[i] = scanline[i] + (precon[i] >> 1u);
            for (i = bytewidth; i < length; ++i)
                recon[i] = scanline[i] + ((recon[i - bytewidth] + precon[i]) >> 1u);
        }
        else
        {
            for (i = 0; i != bytewidth; ++i)
                recon[i] = scanline[i];
            for (i = bytewidth; i < length; ++i)
                recon[i] = scanline[i] + (recon[i - bytewidth] >> 1u);
        }
        break;
    case Png::FT_Paeth:
        if (precon)
        {
            for (i = 0; i != bytewidth; ++i)
            {
                recon[i] = (scanline[i] + precon[i]); /*paethPredictor(0, precon[i], 0) is always precon[i]*/
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
                    recon[i + 0] = s0 + paethPredictor(r0, p0, q0);
                    recon[i + 1] = s1 + paethPredictor(r1, p1, q1);
                }
            }

            for (; i != length; ++i)
            {
                recon[i] = (scanline[i] + paethPredictor(recon[i - bytewidth], precon[i], precon[i - bytewidth]));
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
                /*paethPredictor(recon[i - bytewidth], 0, 0) is always recon[i - bytewidth]*/
                recon[i] = (scanline[i] + recon[i - bytewidth]);
            }
        }
        break;
    default:
        logger_error("Invalid filter type: %u", filterType);
        return Result::ERR_INVALID_DATA; /*error: nonexistent filter type given*/
    }
    return Result::SUCCESS;
}

Result Graphic::PngReader::uncompress(IO::MemoryWriter &uncompressed_writer)
{
    // Decode our compressed image data
    Compression::Inflate inflate;
    IO::MemoryReader compressed_reader(Slice(_idat_writer.slice()));
    return inflate.perform(compressed_reader, uncompressed_writer).result();
}

Result Graphic::PngReader::convert(uint8_t *buffer)
{
    _pixels.resize(_width * _height);

    if (_colour_type == Png::CT_RGBA)
    {
        auto rgba_data = buffer;

        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = Color::from_rgba_byte(rgba_data[i * 4],
                                               rgba_data[i * 4 + 1],
                                               rgba_data[i * 4 + 2],
                                               rgba_data[i * 4 + 3]);
        }
    }
    // It's RGB data convert it to RGBA
    else if (_colour_type == Png::CT_RGB)
    {
        auto rgb_data = buffer;

        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = Color::from_rgb_byte(rgb_data[i * 3],
                                              rgb_data[i * 3 + 1],
                                              rgb_data[i * 3 + 2]);
        }
    }
    // It's grayscale
    else if (_colour_type == Png::CT_GREY)
    {
        auto monochrome_data = buffer;

        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = Color::from_monochrome_byte(monochrome_data[i]);
        }
    }
    else if (_colour_type == Png::CT_GREY_ALPHA)
    {
        auto monochrome_alpha_data = buffer;

        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = Color::from_monochrome_alpha_byte(monochrome_alpha_data[i * 2],
                                                           monochrome_alpha_data[i * 2 + 1]);
        }
    }
    else if (_colour_type == Png::CT_PALETTE)
    {
        if (_palette.empty())
        {
            logger_error("Palette colour type requires a palett data");
            return Result::ERR_INVALID_DATA;
        }

        auto palette_data = buffer;

        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = _palette[palette_data[i]];
        }
    }
    else
    {
        logger_error("Unsupported PNG colour type: %u", _colour_type);
        return Result::ERR_NOT_IMPLEMENTED;
    }

    return Result::SUCCESS;
}