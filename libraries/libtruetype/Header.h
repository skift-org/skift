#pragma once

#include <libutils/Endian.h>

namespace truetype
{

struct Element
{
    uint8_t tag[4];
    be_uint32_t checksum;
    be_uint32_t offset;
    be_uint32_t length;

    bool match(const char *other) const
    {
        return tag[0] == other[0] &&
               tag[1] == other[1] &&
               tag[2] == other[2] &&
               tag[3] == other[3];
    }
};

struct Header
{
    be_uint32_t version;

    be_uint16_t table_count;
    be_uint16_t search_range;
    be_uint16_t entry_selector;
    be_uint16_t range_shift;

    Element tables[];
};

} // namespace truetype
