#pragma once
#include <libsystem/Common.h>
#include <libutils/Vector.h>

class BitReader
{
public:
    inline BitReader(const Vector<uint8_t> &data) : _data(data.raw_storage()), _size(data.count())
    {
    }

    inline BitReader(uint8_t *data, uint32_t size) : _data(data), _size(size)
    {
    }

    inline void skip_bits(uint32_t num_bits)
    {
        _index += num_bits;
    }

    inline uint16_t grab_short()
    {
        uint16_t result = _data[_index / 8] + (_data[(_index / 8) + 1] << 8);
        _index += 16;
        return result;
    }

    inline uint8_t peek_bit()
    {
        return (_data[_index / 8] & (1 << (_index % 8))) ? 1 : 0;
    }

    inline unsigned int grab_bits(unsigned int num_bits)
    {
        unsigned int ret_val = 0;
        for (unsigned int i = 0; i != num_bits; i++)
        {
            ret_val |= (unsigned int)peek_bit() << i;
            _index++;
        }
        return ret_val;
    }

    inline unsigned int peek_bits(unsigned int num_bits)
    {
        unsigned int cached_index = _index;
        unsigned int result = grab_bits(num_bits);
        _index = cached_index;
        return result;
    }

    inline unsigned int grab_bits_reverse(unsigned int num_bits)
    {
        unsigned int ret_val = 0;
        for (unsigned int i = 0; i != num_bits; i++)
        {
            ret_val |= (unsigned int)peek_bit() << ((num_bits - 1) - i);
            _index++;
        }
        return ret_val;
    }

    inline unsigned int peek_bits_reverse(unsigned int num_bits)
    {
        unsigned int cached_index = _index;
        unsigned int result = grab_bits_reverse(num_bits);
        _index = cached_index;
        return result;
    }

private:
    const uint8_t *_data;
    const uint32_t _size;
    uint32_t _index = 0;
};