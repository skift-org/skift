#pragma once
#include <libsystem/Common.h>
#include <libsystem/Result.h>
#include <libutils/HashMap.h>
#include <libutils/Vector.h>

// See https://tools.ietf.org/html/rfc1951#section-3.2.3
enum BlockType
{
    BT_UNCOMPRESSED = 0,
    BT_FIXED_HUFFMAN = 1,
    BT_DYNAMIC_HUFFMAN = 2,
};

class Inflate
{
public:
    Result Perform(Vector<uint8_t> input, Vector<uint8_t> output);

private:
    void buildFixedHuffmanAlphabet();
    void buildHuffmanAlphabet(Vector<unsigned int> &alphabet, Vector<unsigned int> codeBitLengths);

    void getBitLengthCount(HashMap<unsigned int, unsigned int> &bitLengthCount, Vector<unsigned int> &codeBitLengths);
    void getFirstCode(HashMap<unsigned int, unsigned int> &firstCodes, HashMap<unsigned int, unsigned int> &bitLengthCount);
    void assignHuffmanCodes(Vector<unsigned int> assignedCodes, Vector<unsigned int> &codeBitLengths, HashMap<unsigned int, unsigned int> &firstCodes);
    unsigned int decodeHuffman(BitStream input, Vector<unsigned int> &alphabet, Vector<unsigned int> &codeBitLengths);

    Vector<unsigned int> _fixedAlphabet;
    Vector<unsigned int> _fixedCodeBitLengths;
    Vector<unsigned int> _fixedDistAlphabet;
    Vector<unsigned int> _fixedDistCodeBitLengths;
};