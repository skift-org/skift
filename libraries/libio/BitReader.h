#pragma once
#include <libio/Read.h>
#include <libsystem/Common.h>
#include <libtest/AssertEqual.h>
#include <libtest/AssertGreaterThan.h>
#include <libtest/AssertLowerEqual.h>

namespace IO
{
class BitReader
{
public:
    inline BitReader(IO::Reader &reader) : _reader(reader)
    {
    }

    inline void skip_bits(size_t num_bits)
    {
        ensure_buffer(num_bits);
        _bit_index += num_bits;
        pop_cache();
    }

    inline uint16_t grab_uint16()
    {
        assert_equal(_bit_index, 0);
        return IO::read<uint16_t>(_reader).value();
    }

    inline unsigned int grab_bits(unsigned int num_bits)
    {
        unsigned int ret_val = 0;
        ensure_buffer(num_bits);
        for (unsigned int i = 0; i != num_bits; i++)
        {
            ret_val |= (unsigned int)peek_bit(_bit_index) << i;
            _bit_index++;
        }
        pop_cache();
        return ret_val;
    }

    inline unsigned int peek_bits(size_t num_bits)
    {
        unsigned int ret_val = 0;
        ensure_buffer(num_bits);
        for (unsigned int i = 0; i != num_bits; i++)
        {
            ret_val |= (unsigned int)peek_bit(_bit_index + i) << i;
        }
        return ret_val;
    }

    inline unsigned int grab_bits_reverse(size_t num_bits)
    {
        unsigned int ret_val = 0;
        ensure_buffer(num_bits);
        for (unsigned int i = 0; i != num_bits; i++)
        {
            ret_val |= (unsigned int)peek_bit(_bit_index) << ((num_bits - 1) - i);
            _bit_index++;
        }
        pop_cache();
        return ret_val;
    }

    inline unsigned int peek_bits_reverse(size_t num_bits)
    {
        unsigned int ret_val = 0;
        ensure_buffer(num_bits);
        for (unsigned int i = 0; i != num_bits; i++)
        {
            ret_val |= (unsigned int)peek_bit(_bit_index + i) << ((num_bits - 1) - i);
        }
        return ret_val;
    }

private:
    inline void ensure_buffer(size_t num_bits)
    {
        auto num_bytes = __align_up(num_bits + _bit_index, 8) / 8;
        
        while (_cached.count() < num_bytes)
        {
            _cached.push_back(IO::read<uint8_t>(_reader).value());
        }
    }

    inline void pop_cache()
    {
        while (_bit_index >= 8)
        {
            _cached.pop();
            _bit_index -= 8;
        }

        assert_lower_equal(_cached.count(), 1);
    }

    inline uint8_t peek_bit(size_t index)
    {
        assert_greater_than(_cached.count(), index / 8);
        const auto &cur_byte = _cached[index / 8];
        return (cur_byte & (1 << (index % 8))) ? 1 : 0;
    }

    Vector<uint8_t> _cached;
    IO::Reader &_reader;
    size_t _bit_index = 0;
};
} // namespace IO