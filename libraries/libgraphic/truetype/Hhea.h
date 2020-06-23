#pragma once

#include <libgraphic/truetype/Types.h>

#define TRUETYPE_HHEAD_TABLE_NAME "hhea"

typedef __big_endian struct
{
    uint16_t major_version;
    uint16_t minor_version;

    int16_t ascender;
    int16_t descender;
    int16_t linegap;

    uint16_t max_advance;
    int16_t min_left_bearing;
    int16_t min_right_bearing;
    int16_t max_extend;

    int16_t caret_slope_rise;
    int16_t caret_slope_run;
    int16_t caret_offset;

    int16_t reserved[4];

    int16_t metric_data_format;
    uint16_t number_of_h_metrics;
} TrueTypeHheadTable;
