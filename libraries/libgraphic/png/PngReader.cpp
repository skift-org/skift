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

    size_t idat_counter = 0;
    bool end = false;
    Png::ColourType colourType = Png::CT_RGB;
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
            logger_trace("FM: %u", image_header.filter_method());
            logger_trace("IM: %u", image_header.interlace_method());
            assert_equal(image_header.bit_depth(), 8);
            colourType = image_header.colour_type();
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
            TRY(IO::read<Png::Time>(_reader));
        }
        break;

        case Png::ImageData::SIG:
        {
            size_t data_size = chunk_length();

            // The first chunk begins with the compression method
            if (idat_counter == 0)
            {
                data_size -= 2;
                // Two bytes before the actual deflate data, see https://www.w3.org/TR/2003/REC-PNG-20031110/#10Compression
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

        case Png::ImageEnd::SIG:
        {
            end = true;
        }
        break;

        default:
        {
            logger_error("Invalid PNG chunk: %u", chunk_signature());
            Vector<uint8_t> data(chunk_length());
            TRY(_reader.read(data.raw_storage(), chunk_length()));
        }
        break;
        }

        auto crc = TRY(IO::read<be_uint32_t>(_reader));
        __unused(crc);
        prev_signature = chunk_signature();
    }

    IO::MemoryWriter uncompressed_writer;
    TRY(uncompress(uncompressed_writer));
    TRY(convert(colourType, uncompressed_writer.buffer()));

    return Result::SUCCESS;
}

Result Graphic::PngReader::uncompress(IO::MemoryWriter &uncompressed_writer)
{
    // Decode our compressed image data
    Compression::Inflate inflate;
    IO::MemoryReader compressed_reader(Slice(_idat_writer.slice()));
    return inflate.perform(compressed_reader, uncompressed_writer).result();
}

Result Graphic::PngReader::convert(Png::ColourType type, uint8_t *buffer)
{
    _pixels.resize(_width * _height);

    if (type == Png::CT_RGBA)
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
    else if (type == Png::CT_RGB)
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
    else if (type == Png::CT_GREY)
    {
        auto monochrome_data = buffer;

        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = Color::from_monochrome_byte(monochrome_data[i]);
        }
    }
    else if (type == Png::CT_GREY_ALPHA)
    {
        auto monochrome_alpha_data = buffer;

        for (size_t i = 0; i < _width * _height; i++)
        {
            _pixels[i] = Color::from_monochrome_alpha_byte(monochrome_alpha_data[i * 2],
                                                           monochrome_alpha_data[i * 2 + 1]);
        }
    }
    else
    {
        logger_error("Unsupported PNG colour type: %u", type);
        return Result::ERR_OPERATION_NOT_SUPPORTED;
    }

    return Result::SUCCESS;
}