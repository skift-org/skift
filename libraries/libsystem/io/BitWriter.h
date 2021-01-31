#pragma once
#include <libsystem/Common.h>
#include <libsystem/io/Stream.h>
#include <libutils/Vector.h>

class BitWriter
{
public:
    BitWriter(Vector<uint8_t> &data) : _data(data)
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
        _data.push_back_data(data, len);
    }

    inline void put_uint16(uint16_t v)
    {
        _data.push_back(v & 0xFF);
        _data.push_back(v >> 8);
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
            _data.push_back(_bit_buffer);
            _bit_count -= 8;
            _bit_buffer >>= 8;
        }
    }

private:
    uint_fast32_t _bit_buffer;
    uint8_t _bit_count;
    size_t _index;
    Vector<uint8_t> &_data;
};