#include <libgraphic/io/PngCommon.h>
#include <libgraphic/io/PngReader.h>
#include <libio/Read.h>
#include <libsystem/Logger.h>
#include <libcompression/Inflate.h>
#include <libutils/Array.h>

graphic::PngReader::PngReader(IO::Reader &reader) : _reader(reader)
{
    Array<uint8_t, 8> signature;
    _reader.read(signature.raw_storage(), sizeof(signature));

    // Doesn't matter if the read above can't read all bytes, this will fail anyways
    if (signature != Array<uint8_t, 8>{137, 80, 78, 71, 13, 10, 26, 10})
    {
        logger_error("Invalid PNG signature!");
        return;
    }

    bool end = false;
    while (!end)
    {
        auto chunk_length = IO::get<be_uint32_t>(_reader);
        auto chunk_signature = IO::get<be_uint32_t>(_reader);

        logger_trace("Read PNG chunk (length: %u, sig: %u)", chunk_length(), chunk_signature());
        switch (chunk_signature())
        {
        case ImageHeader::SIG:
        {
            auto image_header = IO::get<ImageHeader>(_reader);
            _width = image_header.width();
            _height = image_header.height();
            logger_trace("Compression method: %u", image_header.compression_method);
        }
        break;

        case Gamma::SIG:
        {
            IO::get<Gamma>(_reader);
        }
        break;

        case Chroma::SIG:
        {
            IO::get<Chroma>(_reader);
        }
        break;

        case BackgroundColor::SIG:
        {
            char buf[6];
            _reader.read(buf, chunk_length());
        }
        break;

        case Time::SIG:
        {
            IO::get<Time>(_reader);
        }
        break;

        case ImageData::SIG:
        {
            Inflate inflate;

            Vector<uint8_t> data(chunk_length());
            _reader.read(data.raw_storage(), chunk_length());
        }
        break;

        case TextualData::SIG:
        {
            Vector<uint8_t> data(chunk_length());
            _reader.read(data.raw_storage(), chunk_length());
        }
        break;

        case ImageEnd::SIG:
        {
            end = true;
        }
        break;

        default:
        {
            logger_error("Invalid PNG chunk: %u", chunk_signature());
            Vector<uint8_t> data(chunk_length());
            _reader.read(data.raw_storage(), chunk_length());
        }
        break;
        }

        auto crc = IO::get<be_uint32_t>(_reader);
        __unused(crc);
    }

    logger_trace("Image dims: %u %u", _width, _height);
}