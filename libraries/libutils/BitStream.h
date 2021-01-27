#pragma once
#include <libsystem/Common.h>
#include <libutils/Vector.h>

class BitStream
{
public:
    inline BitStream(Vector<uint8_t> data) : _data(data.raw_storage()), _size(data.count())
    {
    }

    inline BitStream(uint8_t *data, uint32_t size) : _data(data), _size(size)
    {
    }

    inline void skip_bits(uint32_t num_bits)
    {
        _index += num_bits;
    }

    // Align our index to byte boundaries
    inline uint8_t grab_byte()
    {
        auto alignment = _index % 8;
        if (alignment > 0)
            _index += (8 - alignment);

        uint8_t byte = _index / 8;
        _index += 8;
        return byte;
    }

    inline uint16_t grab_short()
    {
        return grab_byte() + (grab_byte() << 8);
    }

    inline bool peek_bit()
    {
        return (_data[_index / 8] & (1 << (_index % 8)));
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
        unsigned int retVal = 0;
        for (unsigned int i = 0; i != num_bits; i++)
        {
            retVal |= (unsigned int)peek_bit() << ((num_bits - 1) - i);
            _index++;
        }
        return retVal;
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