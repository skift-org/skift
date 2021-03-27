#pragma once

#include <libutils/Endian.h>

namespace Graphic::Png
{

enum CompressionMethod : uint8_t
{
    CM_Inflate = 0
};

using be_cm = LittleEndian<CompressionMethod>;

enum ColourType : uint8_t
{
    CT_GREY = 0,       /*grayscale: 1,2,4,8,16 bit*/
    CT_RGB = 2,        /*RGB: 8,16 bit*/
    CT_PALETTE = 3,    /*palette: 1,2,4,8 bit*/
    CT_GREY_ALPHA = 4, /*grayscale with alpha: 8,16 bit*/
    CT_RGBA = 6
};

using be_ct = LittleEndian<ColourType>;

struct __packed ImageHeader
{
    // iHDR
    static constexpr uint32_t SIG = 0x49484452;
    be_uint32_t width;
    be_uint32_t height;
    be_uint8_t bit_depth;
    be_ct colour_type;
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

struct __packed PhysicalDimensions
{
    // pHYs
    static constexpr uint32_t SIG = 0x70485973;
    be_uint32_t pixels_per_x;
    be_uint32_t pixels_per_y;
    be_uint8_t unit_specifier;
};

struct __packed sRGB
{
    // sRGB
    static constexpr uint32_t SIG = 0x73524742;
    be_uint8_t rendering_intent;
};

struct __packed Palette
{
    // PLTE
    static constexpr uint32_t SIG = 0x504C5445;
};

struct Transparency
{
    // tRNS
    static constexpr uint32_t SIG = 0x74524E53;
};

enum FilterType : uint8_t
{
    FT_None = 0,
    FT_Sub = 1,
    FT_Up = 2,
    FT_Average = 3,
    FT_Paeth = 4
};

} // namespace Graphic::Png
