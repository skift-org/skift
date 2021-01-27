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

    inline void SkipBits(uint32_t numBits)
    {
        _index += numBits;
    }

    // Align our index to byte boundaries
    inline uint8_t GrabByte()
    {
        auto alignment = _index % 8;
        if (alignment > 0)
            _index += (8 - alignment);

        uint8_t byte = _index / 8;
        _index += 8;
        return byte;
    }

    inline uint16_t GrabShort()
    {
        return GrabByte() + (GrabByte() << 8);
    }

    inline bool PeekBit()
    {
        return (_data[_index / 8] & (1 << (_index % 8)));
    }

    inline unsigned int GrabBits(unsigned int numBits)
    {
        unsigned int retVal = 0;
        for (unsigned int i = 0; i != numBits; i++)
        {
            retVal |= (unsigned int)PeekBit() << i;
            _index++;
        }
        return retVal;
    }

    inline unsigned int PeekBits(unsigned int numBits)
    {
        unsigned int cachedIndex = _index;
        unsigned int result = GrabBits(numBits);
        _index = cachedIndex;
        return result;
    }

    inline unsigned int GrabBitsReverse(unsigned int numBits)
    {
        unsigned int retVal = 0;
        for (unsigned int i = 0; i != numBits; i++)
        {
            retVal |= (unsigned int)PeekBit() << ((numBits - 1) - i);
            _index++;
        }
        return retVal;
    }

    inline unsigned int PeekBitsReverse(unsigned int numBits)
    {
        unsigned int cachedIndex = _index;
        unsigned int result = GrabBitsReverse(numBits);
        _index = cachedIndex;
        return result;
    }

private:
    const uint8_t *_data;
    const uint32_t _size;
    uint32_t _index = 0;
};