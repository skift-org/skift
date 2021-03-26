#pragma once
#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libutils/Array.h>

namespace Compression
{
class CRC
{
private:
    static const Array<uint32_t, 256> _table;
    uint32_t _crc = 0;

public:
    inline CRC(uint32_t crc = 0) : _crc(crc)
    {
    }

    inline void add(const uint8_t *data, size_t size)
    {
        _crc = ~_crc;

        for (size_t i = 0; i < size; ++i)
        {
            _crc = _table[data[i] ^ (uint8_t)_crc] ^ (_crc >> 8);
        }

        _crc = ~_crc;
    }

    inline uint32_t checksum()
    {
        return _crc;
    }

private:
    template <typename T, int N>
    static inline constexpr Array<T, N> calculate_table()
    {
        Array<T, N> lookup;
        const unsigned long POLYNOMIAL = 0xEDB88320;
        unsigned long remainder = 0;
        unsigned char b = 0;
        do
        {
            // Start with the data byte
            remainder = b;
            for (unsigned long bit = 8; bit > 0; --bit)
            {
                if (remainder & 1)
                    remainder = (remainder >> 1) ^ POLYNOMIAL;
                else
                    remainder = (remainder >> 1);
            }
            lookup[(size_t)b] = remainder;
        } while (0 != ++b);

        return lookup;
    }
};
} // namespace Compression