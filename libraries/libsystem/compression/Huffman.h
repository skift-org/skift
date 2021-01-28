#pragma once
#include <libsystem/Assert.h>
#include <libsystem/io/Stream.h>
#include <libutils/BitStream.h>

class HuffmanDecoder
{
public:
    inline HuffmanDecoder(Vector<unsigned int> &alphabet, Vector<unsigned int> &code_bit_lengths) : _alphabet(alphabet), _code_bit_lengths(code_bit_lengths)
    {
        printf("Build huffman decoder with alphabet size: %u | code_bit_lengths size: %u\n", _alphabet.count(), _code_bit_lengths.count());

        // printf("Alphabet:\n");
        // _alphabet.foreach([](unsigned int v) {
        //     printf("%u_",v);
        //     return Iteration::CONTINUE;
        // });
        // printf("\n");
    }

    unsigned int decode(BitStream &input)
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
                printf("Decoded symbol: i:%u code:%u\n", i, _alphabet[i]);
                input.grab_bits(cbl);
                return i;
            }
        }

        return 0;
    }

private:
    const Vector<unsigned int> &_alphabet;
    const Vector<unsigned int> &_code_bit_lengths;
};