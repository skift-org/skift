#pragma once

#include <libgraphic/truetype/Types.h>

#define TRUETYPE_LOCA_TABLE_NAME "loca"

typedef __big_endian struct
{
    uint16_t offsets[];
} TrueTypeLocalTableV1;

typedef __big_endian struct
{
    uint32_t offsets[];
} TrueTypeLocalTableV2;
