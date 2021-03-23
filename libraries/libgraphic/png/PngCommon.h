#pragma once
#include <libutils/Endian.h>

namespace Png
{

enum CompressionMethod : uint8_t
{
    CM_Inflate = 0
};

using be_cm = LittleEndian<CompressionMethod>;

struct __packed ImageHeader
{
    // iHDR
    static constexpr uint32_t SIG = 0x49484452;
    be_uint32_t width;
    be_uint32_t height;
    be_uint8_t bit_depth;
    be_uint8_t colour_type;
    be_cm compression_method;
    be_uint8_t filter_method;
    be_uint8_t interlace_method;
};

struct __packed Gamma
{
    // gAMA
    static constexpr uint32_t SIG = 0x67414D41;
    be_uint32_t gamma;
};

struct __packed Chroma
{
    // cHRM
    static constexpr uint32_t SIG = 0x6348524D;
    be_uint32_t white_point_x;
    be_uint32_t white_point_y;
    be_uint32_t red_x;
    be_uint32_t red_y;
    be_uint32_t green_x;
    be_uint32_t green_y;
    be_uint32_t blue_x;
    be_uint32_t blue_y;
};

struct __packed BackgroundColor
{
    // bKGD
    static constexpr uint32_t SIG = 0x624B4744;
};

struct __packed Time
{
    // tIME
    static constexpr uint32_t SIG = 0x74494D45;
    be_uint16_t year;
    be_uint8_t month;
    be_uint8_t day;
    be_uint8_t hour;
    be_uint8_t minute;
    be_uint8_t second;
};

struct __packed ImageData
{
    // iDAT
    static constexpr uint32_t SIG = 0x49444154;
};

struct __packed TextualData
{
    // tEXt
    static constexpr uint32_t SIG = 0x74455874;
};

struct __packed ImageEnd
{
    // iEND
    static constexpr uint32_t SIG = 0x49454E44;
};
} // namespace Png