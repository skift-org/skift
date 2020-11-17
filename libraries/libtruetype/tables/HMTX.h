#pragma once

namespace truetype
{

struct LongHorMetric
{
    tt_uint16_t advanceWidth;
    tt_int16_t left_side_bearing;
};

struct HMTX
{
    static constexpr const char *NAME = "hmtx";
};

} // namespace truetype
