#pragma once

#include <libio/Read.h>
#include <libutils/Endian.h>

namespace IO
{

constexpr uint32_t make_tag(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
{
    return a + ((uint32_t)b << 8) + ((uint32_t)c << 16) + ((uint32_t)d << 24);
}

const auto read_tag = read<le_uint32_t>;

} // namespace IO
