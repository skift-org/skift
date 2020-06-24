#pragma once

#include <libgraphic/truetype/TrueType.h>

#define TRUETYPE_CMAP_TABLE_NAME "cmap"

typedef __big_endian struct
{
    uint16_t version;
    uint16_t subtable_count;
} TrueTypeCmapTable;

typedef __big_endian struct
{
#define TRUETYPE_CMAP_UNICODE 0
#define TRUETYPE_CMAP_MACINTOSH 1
#define TRUETYPE_CMAP_MICROSOFT 2
    uint16_t platform;
    uint16_t platform_specific;
    uint32_t offset;
} TrueTypeCmapSubTable;
