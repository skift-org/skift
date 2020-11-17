#pragma once

#include <libtruetype/Types.h>

namespace truetype
{

struct EncodingRecord
{
    tt_uint16_t platform_id;
    tt_uint16_t encoding_id;
    tt_offset32_t subtable_offset;
};

struct CMAP
{
    static constexpr const char *NAME = "cmap";

    tt_uint16_t version;
    tt_uint16_t num_tables;
};

} // namespace truetype
