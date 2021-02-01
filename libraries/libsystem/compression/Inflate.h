#pragma once
#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libsystem/io/BitReader.h>
#include <libutils/HashMap.h>
#include <libutils/Vector.h>

class Reader;
class Writer;
class Inflate
{
public:
    Result perform(Reader &compressed, Writer &uncompressed);

private:
    void build_fixed_huffman_alphabet();
    Result build_dynamic_huffman_alphabet(BitReader &input);
    void build_huffman_alphabet(Vector<unsigned int> &alphabet, const Vector<unsigned int> &code_bit_lengths);

    void get_bit_length_count(HashMap<unsigned int, unsigned int> &bit_length_count, const Vector<unsigned int> &code_bit_lengths);
    void get_first_code(HashMap<unsigned int, unsigned int> &firstCodes, HashMap<unsigned int, unsigned int> &bit_length_count);
    void assign_huffman_codes(Vector<unsigned int> &assigned_codes, const Vector<unsigned int> &code_bit_lengths, HashMap<unsigned int, unsigned int> &first_codes);

    // Fixed huffmann
    Vector<unsigned int> _fixed_alphabet;
    Vector<unsigned int> _fixed_code_bit_lengths;
    Vector<unsigned int> _fixed_dist_alphabet;
    Vector<unsigned int> _fixed_dist_code_bit_lengths;

    // Dynamic huffmann
    Vector<unsigned int> _lit_len_alphabet;
    Vector<unsigned int> _lit_len_code_bit_length;
    Vector<unsigned int> _dist_code_bit_length;
    Vector<unsigned int> _dist_alphabet;
};