#pragma once

#include <libutils/Prelude.h>

struct PACKED MBREntry
{
    uint8_t attributes;
    uint8_t chs_start[3];
    uint8_t type;
    uint8_t chs_end[3];

    uint32_t start;
    uint32_t size;
};

static_assert(sizeof(MBREntry) == 16);

struct PACKED MBR
{
    uint8_t bootstrap[440];

    uint32_t signature;
    uint16_t reserverd;

    MBREntry entries[4];

    uint16_t magic;
};

static_assert(sizeof(MBR) == 512);
