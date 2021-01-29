#pragma once
#include <libsystem/Common.h>
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

    inline void put_bits(unsigned int v, const unsigned int num_bits)
    {
        _bit_buffer |= v << _bit_count;
        _bit_count += num_bits;
    }

    inline void put_data(uint8_t *data, unsigned int len)
    {
        _data.push_back_many(Vector<uint8_t>(ADOPT, data, len));
    }

    inline void put_short(unsigned short v)
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
        while (_bit_count > 8)
        {
            _data.emplace_back() = _bit_buffer;
            _bit_count -= 8;
            _bit_buffer >>= 8;
        }
    }

private:
    unsigned int _bit_buffer;
    unsigned int _bit_count;
    unsigned int _index;
    Vector<uint8_t> &_data;
};