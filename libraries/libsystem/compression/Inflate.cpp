#include <libsystem/compression/BitStream.h>
#include <libsystem/compression/Inflate.h>

static constexpr unsigned int BASE_LENGTH_EXTRA_BITS[] = {
    0, 0, 0, 0, 0, 0, 0, 0, //257 - 264
    1, 1, 1, 1,             //265 - 268
    2, 2, 2, 2,             //269 - 273
    3, 3, 3, 3,             //274 - 276
    4, 4, 4, 4,             //278 - 280
    5, 5, 5, 5,             //281 - 284
    0                       //285
};

static constexpr int BASE_LENGTHS[] = {
    3, 4, 5, 6, 7, 8, 9, 10, //257 - 264
    11, 13, 15, 17,          //265 - 268
    19, 23, 27, 31,          //269 - 273
    35, 43, 51, 59,          //274 - 276
    67, 83, 99, 115,         //278 - 280
    131, 163, 195, 227,      //281 - 284
    258                      //285
};

static constexpr unsigned int BASE_DISTANCE[] = {
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
    0, 0          //30-31, error, shouldn't occur
};

static constexpr unsigned int BASE_DISTANCE_EXTRA_BITS[] = {
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
    0, 0        //30-31 error, they shouldn't occur
};

void Inflate::getBitLengthCount(HashMap<unsigned int, unsigned int> &bitLengthCount, Vector<unsigned int> &codeBitLengths)
{
    for (unsigned int i = 0; i != codeBitLengths.count(); i++)
        bitLengthCount[codeBitLengths[i]] = 0;

    for (unsigned int i = 0; i != codeBitLengths.count(); i++)
        bitLengthCount[codeBitLengths[i]]++;
}

void Inflate::getFirstCode(HashMap<unsigned int, unsigned int> &firstCodes, HashMap<unsigned int, unsigned int> &bitLengthCount)
{
    unsigned int code = 0;
    unsigned int prevBLCount = 0;
    for (unsigned int i = 1; i <= (unsigned int)bitLengthCount.count(); i++)
    {
        if (i >= 2)
            prevBLCount = bitLengthCount[i - 1];
        code = (code + prevBLCount) << 1;
        firstCodes[i] = code;
    }
}

void Inflate::assignHuffmanCodes(Vector<unsigned int> assignedCodes, Vector<unsigned int> &codeBitLengths, HashMap<unsigned int, unsigned int> &firstCodes)
{
    assignedCodes.resize(codeBitLengths.count());

    for (unsigned int i = 0; i < (unsigned int)codeBitLengths.count(); i++)
        if (codeBitLengths[i])
            assignedCodes[i] = firstCodes[codeBitLengths[i]]++;
        else
            assignedCodes[i] = 0;
}

void Inflate::buildHuffmanAlphabet(Vector<unsigned int> &alphabet, Vector<unsigned int> codeBitLengths)
{
    HashMap<unsigned int, unsigned int> bitLengthCount, firstCodes;

    getBitLengthCount(bitLengthCount, codeBitLengths);
    getFirstCode(firstCodes, bitLengthCount);
    assignHuffmanCodes(alphabet, codeBitLengths, firstCodes);
}

void Inflate::buildFixedHuffmanAlphabet()
{
    if (_fixedCodeBitLengths.count())
        return;

    _fixedCodeBitLengths.resize(288);
    _fixedDistCodeBitLengths.resize(32);

    for (int i = 0; i <= 287; i++)
    {
        if (i >= 0 && i <= 143)
            _fixedCodeBitLengths[i] = 8;
        else if (i >= 144 && i <= 255)
            _fixedCodeBitLengths[i] = 9;
        else if (i >= 256 && i <= 279)
            _fixedCodeBitLengths[i] = 7;
        else if (i >= 280 && i <= 287)
            _fixedCodeBitLengths[i] = 8;
    }

    for (int i = 0; i != 32; i++)
        _fixedDistCodeBitLengths[i] = 5;

    buildHuffmanAlphabet(_fixedAlphabet, _fixedCodeBitLengths);
    buildHuffmanAlphabet(_fixedDistAlphabet, _fixedDistCodeBitLengths);
}

unsigned int Inflate::decodeHuffman(BitStream input, Vector<unsigned int> &alphabet, Vector<unsigned int> &codeBitLengths)
{
    for (unsigned int i = 0; i < (unsigned int)codeBitLengths.count(); i++)
    {
        if (codeBitLengths[i] == 0)
            continue;
        unsigned int code = input.PeekBitsReverse(codeBitLengths[i]);
        if (alphabet[i] == code)
        {
            input.GrabBits(codeBitLengths[i]);
            return i;
        }
    }

    return 0;
}

Result Inflate::Perform(Vector<uint8_t> inputData, Vector<uint8_t> output)
{
    BitStream input(inputData);
    // ZLib Header
    input.GrabBits(16);

    Vector<unsigned int> codeLengthOfCodeLengthOrder = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
    Vector<unsigned int> codeLengthOfCodeLength = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    while (true)
    {
        unsigned int bfinal = input.GrabBits(1);
        __unused(bfinal);
        unsigned int btype = input.GrabBits(2);

        // Uncompressed block
        if (btype == BT_UNCOMPRESSED)
        {
            // This aligns to the next byte boundary (skips 5 bits)
            auto len = input.GrabShort();
            // Skip complement of LEN
            input.SkipBits(16);

            for (int i = 0; i != len; i++)
                output.push_back(input.GrabByte());
        }
        else if (btype == BT_FIXED_HUFFMAN ||
                 btype == BT_DYNAMIC_HUFFMAN)
        {
            Vector<unsigned int> litLenCodeBitLength, litLenAlphabet;
            Vector<unsigned int> distCodeBitLength, distAlphabet;

            // Use a fixed huffman alphabet
            if (btype == BT_FIXED_HUFFMAN)
            {
                buildFixedHuffmanAlphabet();
            }
            // Use a dynamic huffman alphabet
            else
            {
                unsigned int hlit = input.GrabBits(5) + 257;
                unsigned int hdist = input.GrabBits(5) + 1;
                unsigned int hclen = input.GrabBits(4) + 4;

                for (unsigned int i = 0; i < hclen; i++)
                {
                    codeLengthOfCodeLength[codeLengthOfCodeLengthOrder[i]] = input.GrabBits(3);
                }

                Vector<unsigned int> litLenAndDistAlphabets;
                buildHuffmanAlphabet(litLenAndDistAlphabets, codeLengthOfCodeLength);

                Vector<unsigned int> litLenAndDistTreesUnpacked;
                while (litLenAndDistTreesUnpacked.count() < (hdist + hlit))
                {
                    unsigned int decodedValue = decodeHuffman(input, litLenAndDistAlphabets, codeLengthOfCodeLength);

                    // Everything below 16 corresponds directly to a codelength. See https://tools.ietf.org/html/rfc1951#section-3.2.7
                    if (decodedValue < 16)
                    {
                        litLenAndDistTreesUnpacked.push_back(decodedValue);
                        continue;
                    }

                    unsigned int repeatCount = 0;
                    unsigned int codeLengthToRepeat = 0;

                    switch (decodedValue)
                    {
                    // 3-6
                    case 16:
                        repeatCount = input.GrabBits(2) + 3;
                        codeLengthToRepeat = litLenAndDistTreesUnpacked[litLenAndDistTreesUnpacked.count() - 1];
                        break;
                    // 3-10
                    case 17:
                        repeatCount = input.GrabBits(3) + 3;
                        break;
                    // 11 - 138
                    case 18:
                        repeatCount = input.GrabBits(7) + 11;
                        break;
                    }

                    for (unsigned int i = 0; i != repeatCount; i++)
                        litLenAndDistTreesUnpacked.push_back(codeLengthToRepeat);
                }

                litLenCodeBitLength.resize(hlit);
                for (unsigned int i = 0; i < litLenCodeBitLength.count(); i++)
                    litLenCodeBitLength[i] = litLenAndDistTreesUnpacked[i];

                distCodeBitLength.resize(litLenAndDistTreesUnpacked.count() - hlit);
                for (unsigned int i = 0; i < distCodeBitLength.count(); i++)
                    distCodeBitLength[i] = litLenAndDistTreesUnpacked[hlit + i];

                buildHuffmanAlphabet(litLenAlphabet, litLenCodeBitLength);
                buildHuffmanAlphabet(distAlphabet, distCodeBitLength);
            }

            while (true)
            {
                unsigned int decodedSymbol = decodeHuffman(input, 
                    btype == BT_FIXED_HUFFMAN ? _fixedAlphabet : litLenAlphabet, 
                    btype == BT_FIXED_HUFFMAN ? _fixedCodeBitLengths : litLenCodeBitLength);
                if (decodedSymbol <= 255)
                {
                    output.push_back((unsigned char)decodedSymbol);
                }
                else if (decodedSymbol == 256)
                {
                    break;
                }
                else
                {
                    unsigned int lengthIndex = decodedSymbol - 257;
                    unsigned int totalLength = BASE_LENGTHS[lengthIndex] + input.GrabBits(BASE_LENGTH_EXTRA_BITS[lengthIndex]);
                    unsigned int distCode = decodeHuffman(input, 
                        btype == BT_FIXED_HUFFMAN ?  _fixedDistAlphabet : distAlphabet, 
                        btype == BT_FIXED_HUFFMAN ? _fixedDistCodeBitLengths : distCodeBitLength);
                    unsigned int totalDist = BASE_DISTANCE[distCode] + input.GrabBits(BASE_DISTANCE_EXTRA_BITS[distCode]);
                    unsigned char *pos = &output[output.count() - totalDist];
                    for (unsigned int i = 0; i != totalLength; i++)
                    {
                        output.push_back(*pos);
                        pos = &output[output.count() - totalDist];
                    }
                }
            }
        }
    }
}