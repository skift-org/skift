#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef char *string;

typedef char s8;
typedef short s16;
typedef long s32;
typedef long long s64;

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

typedef u8 *buffer8_t;
typedef u16 *buffer16_t;
typedef u32 *buffer32_t;

typedef struct
{
    s32 x;
    s32 y;
} spoint_t;

typedef struct
{
    u32 x;
    u32 y;
} upoint_t;