#pragma once

#include <libgraphic/truetype/Types.h>
#include <libsystem/utils/Slice.h>

#define TRUETYPE_KERN_TABLE_NAME "kern"

typedef __big_endian struct
{
    uint16_t version; // = 0 for microsoft
    uint16_t table_count;
} TrueTypeKernTableMS;

typedef __big_endian struct
{
    uint32_t version; // = 0x00010000 for apple
    uint32_t table_count;
} TrueTypeKernTableAPPLE;

typedef __big_endian struct
{
    uint32_t length;

#define TRUETYPE_KERN_VERTICAL 0x8000
#define TRUETYPE_KERN_CROSS_STREAM 0x4000
#define TRUETYPE_KERN_VARIATION 0x2000
#define TRUETYPE_KERN_FORMAT 0x00FF
    uint16_t coverage;
} TrueTypeKernSubTable;

typedef __big_endian struct
{
    TrueTypeKernSubTable header;

    uint16_t pairs_count;
    uint16_t search_range;
    uint16_t entry_selector;
    uint16_t range_shift;

    struct
    {
        uint16_t left;
        uint16_t right;

        int16_t value;
    } pairs[];
} TrueTypeKernFormat0;
