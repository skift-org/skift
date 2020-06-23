#pragma once

#include <libgraphic/truetype/Types.h>

#define TRUETYPE_HEAD_TABLE_NAME "head"

typedef __big_endian struct
{
    uint16_t major_version;
    uint16_t minor_version;
    TT16Dot16 font_revision;
    uint32_t checksum;
    uint32_t magic;
    uint16_t flags;
    uint16_t units_per_em;
    TTDateTime creation_time;
    TTDateTime modified_time;
    int16_t x_min;
    int16_t y_min;
    int16_t x_max;
    int16_t y_max;
    uint16_t mac_style;
    uint16_t smallest_readable_size;
    int16_t direction_hint;
    int16_t loca_format;
    int16_t glyph_format;
} TrueTypeHeadTable;
