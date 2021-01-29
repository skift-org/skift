#pragma once
#include <libsystem/Common.h>
#include <libutils/Vector.h>

class BitWriter
{
public:
    BitWriter(Vector<uint8_t> &data) : _data(data)
    {
    }

private:
    Vector<uint8_t> &_data;
};