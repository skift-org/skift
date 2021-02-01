#pragma once
#include <libsystem/Common.h>
#include <libsystem/io/Writer.h>

class BitWriter
{
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

    inline void put_data(uint8_t *data, size_t len)
    {
        _writer.write(data, len);
    }

    inline void put_uint16(uint16_t v)
    {
        _writer.write_byte(v & 0xFF);
        _writer.write_byte(v >> 8);
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
            _writer.write_byte(_bit_buffer);
            _bit_count -= 8;
            _bit_buffer >>= 8;
        }
    }

private:
    uint_fast32_t _bit_buffer;
    uint8_t _bit_count;
    Writer &_writer;
};