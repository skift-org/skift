#include <libcompression/Common.h>
#include <libcompression/Huffman.h>
#include <libcompression/Inflate.h>
#include <libio/BitReader.h>
#include <libio/BufReader.h>
#include <libio/Copy.h>
#include <libio/MemoryWriter.h>
#include <libio/Skip.h>
#include <libsystem/Logger.h>
#include <libutils/InlineRingBuffer.h>

namespace Compression
{

static constexpr uint8_t BASE_LENGTH_EXTRA_BITS[] = {
    0, 0, 0, 0, 0, 0, 0, 0, //257 - 264
    1, 1, 1, 1,             //265 - 268
    2, 2, 2, 2,             //269 - 273
    3, 3, 3, 3,             //274 - 276
    4, 4, 4, 4,             //278 - 280
    5, 5, 5, 5,             //281 - 284
    0                       //285
};

static constexpr uint16_t BASE_LENGTHS[] = {
    3, 4, 5, 6, 7, 8, 9, 10, //257 - 264
    11, 13, 15, 17,          //265 - 268
    19, 23, 27, 31,          //269 - 273
    35, 43, 51, 59,          //274 - 276
    67, 83, 99, 115,         //278 - 280
    131, 163, 195, 227,      //281 - 284
    258                      //285
};

static constexpr uint16_t BASE_DISTANCE[] = {
    1, 2, 3, 4,   //0-3
    5, 7,         //4-5
    9, 13,        //6-7
    17, 25,       //8-9
    33, 49,       //10-11
    65, 97,       //12-13
    129, 193,     //14-15
    257, 385,     //16-17
    513, 769,     //18-19
    1025, 1537,   //20-21
    2049, 3073,   //22-23
    4097, 6145,   //24-25
    8193, 12289,  //26-27
    16385, 24577, //28-29
};

static constexpr uint8_t BASE_DISTANCE_EXTRA_BITS[] = {
    0, 0, 0, 0, //0-3
    1, 1,       //4-5
    2, 2,       //6-7
    3, 3,       //8-9
    4, 4,       //10-11
    5, 5,       //12-13
    6, 6,       //14-15
    7, 7,       //16-17
    8, 8,       //18-19
    9, 9,       //20-21
    10, 10,     //22-23
    11, 11,     //24-25
    12, 12,     //26-27
    13, 13,     //28-29
};

void Inflate::get_bit_length_count(HashMap<unsigned int, unsigned int> &bit_length_count, const Vector<unsigned int> &code_bit_lengths)
{
    for (unsigned int i = 0; i != code_bit_lengths.count(); i++)
    {
        bit_length_count[code_bit_lengths[i]] = 0;
    }

    for (unsigned int i = 0; i != code_bit_lengths.count(); i++)
    {
        bit_length_count[code_bit_lengths[i]]++;
    }
}

void Inflate::get_first_code(HashMap<unsigned int, unsigned int> &first_codes, HashMap<unsigned int, unsigned int> &bit_length_count)
{
    unsigned int code = 0;
    unsigned int prev_bl_count = 0;
    for (unsigned int i = 1; i <= (unsigned int)bit_length_count.count(); i++)
    {
        if (i >= 2)
        {
            prev_bl_count = bit_length_count[i - 1];
        }
        code = (code + prev_bl_count) << 1;
        first_codes[i] = code;
    }
}

void Inflate::assign_huffman_codes(Vector<unsigned int> &assigned_codes, const Vector<unsigned int> &code_bit_lengths, HashMap<unsigned int, unsigned int> &first_codes)
{
    assigned_codes.resize(code_bit_lengths.count());

    for (unsigned int i = 0; i < (unsigned int)code_bit_lengths.count(); i++)
    {
        if (code_bit_lengths[i])
        {
            assigned_codes[i] = first_codes[code_bit_lengths[i]]++;
        }
        else
        {
            assigned_codes[i] = 0;
        }
    }
}

void Inflate::build_huffman_alphabet(Vector<unsigned int> &alphabet, const Vector<unsigned int> &code_bit_lengths)
{
    HashMap<unsigned int, unsigned int> bit_length_count, first_codes;

    get_bit_length_count(bit_length_count, code_bit_lengths);
    get_first_code(first_codes, bit_length_count);
    assign_huffman_codes(alphabet, code_bit_lengths, first_codes);
}

// TODO: use a constexpr function to calculate these tables
void Inflate::build_fixed_huffman_alphabet()
{
    if (_fixed_code_bit_lengths.count())
    {
        return;
    }

    _fixed_code_bit_lengths.resize(288);
    _fixed_dist_code_bit_lengths.resize(32);

    for (int i = 0; i <= 287; i++)
    {
        if (i >= 0 && i <= 143)
        {
            _fixed_code_bit_lengths[i] = 8;
        }
        else if (i >= 144 && i <= 255)
        {
            _fixed_code_bit_lengths[i] = 9;
        }
        else if (i >= 256 && i <= 279)
        {
            _fixed_code_bit_lengths[i] = 7;
        }
        else if (i >= 280 && i <= 287)
        {
            _fixed_code_bit_lengths[i] = 8;
        }
    }

    for (int i = 0; i != 32; i++)
    {
        _fixed_dist_code_bit_lengths[i] = 5;
    }

    build_huffman_alphabet(_fixed_alphabet, _fixed_code_bit_lengths);
    build_huffman_alphabet(_fixed_dist_alphabet, _fixed_dist_code_bit_lengths);
}

Result Inflate::build_dynamic_huffman_alphabet(IO::BitReader &input)
{
    Vector<unsigned int> code_length_of_code_length_order = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    Vector<unsigned int> code_length_of_code_length = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    unsigned int hlit = input.grab_bits(5) + 257;
    unsigned int hdist = input.grab_bits(5) + 1;
    unsigned int hclen = input.grab_bits(4) + 4;

    // See: https://github.com/madler/zlib/issues/82
    if (hlit > 286 || hdist > 30)
    {
        return Result::ERR_INVALID_DATA;
    }

    for (unsigned int i = 0; i < hclen; i++)
    {
        code_length_of_code_length[code_length_of_code_length_order[i]] = input.grab_bits(3);
    }

    Vector<unsigned int> lit_len_and_dist_alphabets;
    build_huffman_alphabet(lit_len_and_dist_alphabets, code_length_of_code_length);

    Vector<unsigned int> lit_len_and_dist_trees_unpacked;
    HuffmanDecoder huffman(lit_len_and_dist_alphabets, code_length_of_code_length);
    while (lit_len_and_dist_trees_unpacked.count() < (hdist + hlit))
    {
        unsigned int decoded_value = huffman.decode(input);

        // Everything below 16 corresponds directly to a codelength. See https://tools.ietf.org/html/rfc1951#section-3.2.7
        if (decoded_value < 16)
        {
            lit_len_and_dist_trees_unpacked.push_back(decoded_value);
            continue;
        }

        unsigned int repeat_count = 0;
        unsigned int code_length_to_repeat = 0;

        switch (decoded_value)
        {
        // 3-6
        case 16:
            repeat_count = input.grab_bits(2) + 3;
            code_length_to_repeat = lit_len_and_dist_trees_unpacked.peek_back();
            break;
        // 3-10
        case 17:
            repeat_count = input.grab_bits(3) + 3;
            break;
        // 11 - 138
        case 18:
            repeat_count = input.grab_bits(7) + 11;
            break;
        }

        for (unsigned int i = 0; i != repeat_count; i++)
        {
            lit_len_and_dist_trees_unpacked.push_back(code_length_to_repeat);
        }
    }

    _lit_len_code_bit_length.resize(hlit);
    for (unsigned int i = 0; i < _lit_len_code_bit_length.count(); i++)
    {
        _lit_len_code_bit_length[i] = lit_len_and_dist_trees_unpacked[i];
    }

    _dist_code_bit_length.resize(lit_len_and_dist_trees_unpacked.count() - hlit);
    for (unsigned int i = 0; i < _dist_code_bit_length.count(); i++)
    {
        _dist_code_bit_length[i] = lit_len_and_dist_trees_unpacked[hlit + i];
    }

    build_huffman_alphabet(_lit_len_alphabet, _lit_len_code_bit_length);
    build_huffman_alphabet(_dist_alphabet, _dist_code_bit_length);
    return Result::SUCCESS;
}

FLATTEN Result Inflate::read_blocks(IO::Reader &reader, IO::Writer &uncompressed)
{
    // We use this as our sliding window. We should write directly into "uncompressed in the future"
    // And limit the amount of data we keep in our sliding window (Dequeue would be nice)
    IO::MemoryWriter dest_writer{32768};

    uint8_t bfinal;
    IO::BitReader bits{reader};
    do
    {
        bfinal = bits.grab_bits(1);
        uint8_t btype = bits.grab_bits(2);

        // Uncompressed block
        if (btype == BT_UNCOMPRESSED)
        {
            // Align to byte bounadries
            bits.skip_bits(5);

            uint16_t len = TRY(IO::read<uint16_t>(reader));

            // Skip complement of LEN
            TRY(IO::skip(reader, 2));

            // copy the uncompressed data
            TRY(IO::copy(reader, dest_writer, len));
        }
        else if (btype == BT_FIXED_HUFFMAN || btype == BT_DYNAMIC_HUFFMAN)
        {
            // Use a fixed huffman alphabet
            if (btype == BT_FIXED_HUFFMAN)
            {
                build_fixed_huffman_alphabet();
            }
            // Use a dynamic huffman alphabet
            else
            {
                TRY(build_dynamic_huffman_alphabet(bits));
            }

            // Do the actual huffman decoding
            HuffmanDecoder symbol_decoder(btype == BT_FIXED_HUFFMAN ? _fixed_alphabet : _lit_len_alphabet,
                                          btype == BT_FIXED_HUFFMAN ? _fixed_code_bit_lengths : _lit_len_code_bit_length);

            HuffmanDecoder dist_decoder(btype == BT_FIXED_HUFFMAN ? _fixed_dist_alphabet : _dist_alphabet,
                                        btype == BT_FIXED_HUFFMAN ? _fixed_dist_code_bit_lengths : _dist_code_bit_length);

            while (true)
            {
                unsigned int decoded_symbol = symbol_decoder.decode(bits);
                if (decoded_symbol <= 255)
                {
                    // Literal symbol
                    IO::write<uint8_t>(dest_writer, decoded_symbol);
                }
                else if (decoded_symbol >= 257 && decoded_symbol <= 285)
                {
                    // Length code
                    unsigned int length_index = decoded_symbol - 257;
                    unsigned int total_length = BASE_LENGTHS[length_index] + bits.grab_bits(BASE_LENGTH_EXTRA_BITS[length_index]);
                    unsigned int dist_code = dist_decoder.decode(bits);

                    Assert::lower_than(dist_code, 30);

                    unsigned int total_dist = BASE_DISTANCE[dist_code] + bits.grab_bits(BASE_DISTANCE_EXTRA_BITS[dist_code]);

                    for (unsigned int i = 0; i != total_length; i++)
                    {
                        size_t offset = TRY(dest_writer.length()) - total_dist;
                        uint8_t val = dest_writer.buffer()[offset];
                        IO::write<uint8_t>(dest_writer, val);
                    }
                }
                else if (decoded_symbol == 256)
                {
                    // End code
                    break;
                }
                else
                {
                    logger_error("Invalid decoded symbol: %u", decoded_symbol);
                    return Result::ERR_INVALID_DATA;
                }
            }
        }
        else
        {
            logger_error("Invalid block type: %u", btype);
            return Result::ERR_INVALID_DATA;
        }
    } while (!bfinal);

    auto final_reader = IO::MemoryReader(Slice(dest_writer.slice()));
    return IO::copy(final_reader, uncompressed);
}

FLATTEN ResultOr<size_t> Inflate::perform(IO::Reader &compressed, IO::Writer &uncompressed)
{
    IO::ReadCounter counter{compressed};
    TRY(read_blocks(counter, uncompressed));
    return counter.count();
}

} // namespace Compression
