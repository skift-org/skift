#pragma once

#include <libtruetype/Types.h>

namespace truetype
{

struct HHEA
{
    static constexpr const char *NAME = "hhea";

    tt_uint16_t major_version;
    tt_uint16_t minor_version;

    tt_fword_t ascender;
    tt_fword_t descender;
    tt_fword_t linegap;
    tt_ufword_t advance_width_max;

    tt_fword_t min_left_side_bearing;
    tt_fword_t min_right_side_bearing;

    tt_fword_t x_max_extend;

    tt_int16_t caret_slope_rise;
    tt_int16_t caret_slope_run;

    tt_int16_t caret_offset;

    tt_int16_t reserved0;
    tt_int16_t reserved1;
    tt_int16_t reserved2;
    tt_int16_t reserved3;

    tt_int16_t metric_data_format;
    tt_int16_t number_of_hmetrics;
};

} // namespace truetype
