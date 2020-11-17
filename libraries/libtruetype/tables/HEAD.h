#pragma once

#include <libtruetype/Types.h>

namespace truetype
{

struct HEAD
{
    static constexpr const char *NAME = "head";

    tt_uint16_t major_version;
    tt_uint16_t minor_version;

    tt_fixed_t font_revision;
    tt_uint32_t checksum_adjustment;
    tt_uint32_t magic_number;
    tt_uint16_t flags;
    tt_uint16_t units_per_em;

    tt_longdatetime_t created;
    tt_longdatetime_t modified;

    tt_int16_t x_min;
    tt_int16_t y_min;
    tt_int16_t x_max;
    tt_int16_t y_max;

    tt_uint16_t mac_style;
    tt_uint16_t lowerst_rec_ppem;
    tt_int16_t font_direction_hint;

    tt_int16_t index_to_loc_format;
    tt_int16_t glyph_data_format;
};

} // namespace truetype
