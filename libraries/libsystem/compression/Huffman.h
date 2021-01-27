#pragma once
#include <libutils/BitStream.h>

class HuffmanDecoder
{
public:
    inline HuffmanDecoder(Vector<unsigned int> &alphabet, Vector<unsigned int> &code_bit_lengths) : _alphabet(alphabet), _code_bit_lengths(code_bit_lengths)
    {
    }

    unsigned int decode(BitStream &input)
    {
        for (unsigned int i = 0; i < (unsigned int)_code_bit_lengths.count(); i++)
        {
            if (_code_bit_lengths[i] == 0)
                continue;
            unsigned int code = input.peek_bits_reverse(_code_bit_lengths[i]);
            if (_alphabet[i] == code)
            {
                input.grab_bits(_code_bit_lengths[i]);
                return i;
            }
        }

        return 0;
    }

private:
    const Vector<unsigned int> &_alphabet;
    const Vector<unsigned int> &_code_bit_lengths;
};