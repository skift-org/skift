#pragma once

#include <libutils/Endian.h>
#include <libutils/Slice.h>

namespace truetype
{

static uint32_t checksum(Slice &table)
{
    uint32_t sum = 0;

    be_uint32_t *t = (be_uint32_t *)table.start();

    for (size_t i = 0; i < __align_down(table.size(), 4) / 4; i++)
    {
        sum += t[i]();
    }

    size_t remaining_bytes = table.size() - __align_down(table.size(), 4);

    uint32_t remaining_sum = 0;

    for (size_t i = 0; i < remaining_bytes; i++)
    {
        remaining_sum <<= 8;
        remaining_sum += ((uint8_t *)t)[__align_down(table.size(), 4) + i];
    }

    remaining_sum <<= 8 * (4 - remaining_bytes);

    return sum + remaining_sum;
}

} // namespace truetype
