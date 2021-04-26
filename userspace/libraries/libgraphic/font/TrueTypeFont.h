#pragma once
#include <libutils/Endian.h>
#include <stdint.h>

namespace Graphic::Font
{
enum TrueTypePlatform : uint16_t
{
    TT_PLATFORM_UNICODE = 0,
    TT_PLATFORM_MACINTOSH = 1,
    TT_PLATFORM_ISO = 2, // was deprecated
    TT_PLATFORM_WINDOWS = 3,
    TT_PLATFORM_CUSTOM = 4,
};

enum TrueTypeEncodingWindows : uint16_t
{
    TT_ENCODING_WIN_SYMBOL = 0,
    TT_ENCODING_WIN_UNICODE_BMP = 1,
    TT_ENCODING_WIN_SHIFTJIS = 2,
    TT_ENCODING_WIN_PRC = 3,
    TT_ENCODING_WIN_BIG5 = 4,
    TT_ENCODING_WIN_WANSUNG = 5,
    TT_ENCODING_WIN_JOHAB = 6,
    TT_ENCODING_WIN_UNICODE_FULL = 10,
};

enum TrueTypeMappingFormat : uint16_t
{
    TT_MAPPING_FMT_APPLE_BYTE_ENCODING = 0,
    TT_MAPPING_FMT_HIGH_BYTE = 2,
    TT_MAPPING_FMT_SEGMENT = 4,
    TT_MAPPING_FMT_TRIMMED_TABLE = 6,
    TT_MAPPING_FMT_MIXED = 8,
    TT_MAPPING_FMT_TRIMMED_ARRAY = 10,
    TT_MAPPING_FMT_SEGMENTED_COV = 12,
    TT_MAPPING_FMT_MANY_TO_ONE = 13,
    TT_MAPPING_FMT_UNICODE_VARIATIONS = 14
};
} // namespace Graphic::Font