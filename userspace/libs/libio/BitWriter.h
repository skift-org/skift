#pragma once
#include <libio/Write.h>
#include <libutils/Prelude.h>

namespace IO
{
struct BitWriter
{
private:
    uint_fast32_t _bit_buffer;
    uint8_t _bit_count;
    Writer &_writer;

public:
    BitWriter(Writer &writer) : _writer(writer)
    {
    }

    ~BitWriter()
    {
        flush();
    }

    inline void put_bits(unsigned int v, const size_t num_bits)
    {
        _bit_buffer |= v << _bit_count;
        _bit_count += num_bits;
    }

    inline void put_data(const uint8_t *data, size_t len)
    {
        _writer.write(data, len);
    }

    inline void put_uint16(uint16_t v)
    {
        IO::write(_writer, v);
    }

    inline void align()
    {
        _bit_count += -_bit_count & 7;
        flush();
    }

    inline void flush()
    {
        // Flush one byte at a time
        while (_bit_count >= 8)
        {
            IO::write(_writer, (uint8_t)_bit_buffer);
            _bit_count -= 8;
            _bit_buffer >>= 8;
        }
    }
};
} // namespace IO