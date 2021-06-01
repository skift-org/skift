#pragma once
#include <abi/Result.h>
#include <libutils/Array.h>
#include <libutils/Prelude.h>

namespace Compression
{

struct CRCTable : public Array<uint32_t, 256>
{
    static constexpr uint32_t POLYNOMIAL = 0xEDB88320;

    constexpr CRCTable()
    {
        uint32_t remainder = 0;
        uint8_t b = 0;

        do
        {
            remainder = b;

            for (uint8_t bit = 8; bit > 0; --bit)
            {
                if (remainder & 1)
                {
                    remainder = (remainder >> 1) ^ POLYNOMIAL;
                }
                else
                {
                    remainder = (remainder >> 1);
                }
            }

            at(b) = remainder;
        } while (0 != ++b);
    }
};

struct CRC
{
private:
    CRCTable _table;
    uint32_t _crc = 0;

public:
    uint32_t checksum() const { return _crc; }

    CRC(uint32_t crc = 0) : _crc{crc}
    {
    }

    void add(const uint8_t *data, size_t size)
    {
        _crc = ~_crc;

        for (size_t i = 0; i < size; ++i)
        {
            _crc = _table[data[i] ^ (uint8_t)_crc] ^ (_crc >> 8);
        }

        _crc = ~_crc;
    }
};

} // namespace Compression