#pragma once

#include <libio/BitReader.h>

namespace Compression
{

struct HuffmanDecoder
{
private:
    const Vector<unsigned int> &_alphabet;
    const Vector<unsigned int> &_code_bit_lengths;

public:
    inline HuffmanDecoder(Vector<unsigned int> &alphabet, Vector<unsigned int> &code_bit_lengths) : _alphabet(alphabet), _code_bit_lengths(code_bit_lengths)
    {
    }

    unsigned int decode(IO::BitReader &input)
    {
        for (unsigned int i = 0; i < (unsigned int)_code_bit_lengths.count(); i++)
        {
            const auto cbl = _code_bit_lengths[i];
            if (cbl == 0)
            {
                continue;
            }

            unsigned int code = input.peek_bits_reverse(cbl);

            if (_alphabet[i] == code)
            {
                input.grab_bits(cbl);
                return i;
            }
        }

        return 0;
    }
};

} // namespace Compression