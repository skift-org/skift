#pragma once

#include <libcompression/CRC.h>
#include <libio/Reader.h>

namespace IO
{

class CRCReader :
    public Reader
{
private:
    Reader &_reader;
    Compression::CRC _crc;

public:
    CRCReader(Reader &reader, uint32_t crc = 0) : _reader{reader}, _crc{crc}
    {
    }

    inline uint32_t checksum()
    {
        return _crc.checksum();
    }

    ResultOr<size_t> read(void *buffer, size_t size) override
    {
        auto result = TRY(_reader.read(buffer, size));
        _crc.add((uint8_t *)buffer, result);
        return result;
    }
};

} // namespace IO
