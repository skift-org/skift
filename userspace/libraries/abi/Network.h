#pragma once

#include <libsystem/Common.h>

struct MacAddress
{
    uint8_t bytes[6];

    uint8_t operator[](int index) const
    {
        return bytes[index];
    }
};
