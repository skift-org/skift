#pragma once
#include <stdint.h>
#include <libutils/Endian.h>

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

    using be_truetype_platform = BigEndian<TrueTypePlatform>;
}