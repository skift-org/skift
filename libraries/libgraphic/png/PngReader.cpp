#include <libcompression/Inflate.h>
#include <libgraphic/png/PngCommon.h>
#include <libgraphic/png/PngReader.h>
#include <libio/Read.h>
#include <libio/Skip.h>
#include <libsystem/Logger.h>
#include <libtest/AssertLowerEqual.h>
#include <libutils/Array.h>

Graphic::PngReader::PngReader(IO::Reader &reader) : _reader(reader)
{
    read();
    logger_trace("Image dims: %u %u", _width, _height);
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

    bool end = false;
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
            logger_trace("Compression method: %u", image_header.compression_method);
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

            // Two bytes before the actual deflate data, see https://www.w3.org/TR/2003/REC-PNG-20031110/#10Compression
            auto cm_cinfo = TRY(IO::read<uint8_t>(_reader));
            // ZLib compression mode should be DEFLATE
            assert_equal(cm_cinfo & 15, 8);
            // Sliding window should be 32k at max
            assert_lower_equal((cm_cinfo >> 4) & 15, 7);
            auto flags = TRY(IO::read<uint8_t>(_reader));
            __unused(flags);

            Compression::Inflate inflate;
            IO::MemoryWriter memory;
            size_t compressed_size = TRY(inflate.perform(_reader, memory));

            // TODO: fix this
            // Use the CRC to do this ugly hack. Missing should be 4, but for some reason it's 3 sometimes for us
            auto missing = chunk_length() - (compressed_size + 2);
            assert_equal(missing, 4);
            // Skip CRC32
            TRY(IO::skip(_reader, missing));

            logger_trace("Uncompressed PNG data: US: %u", memory.length().value());
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
    }

    return Result::SUCCESS;
}
