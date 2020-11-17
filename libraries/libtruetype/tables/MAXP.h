#pragma once

#include <libtruetype/Types.h>

namespace truetype
{

struct MAXP
{
    static constexpr const char *NAME = "maxp";

    tt_ver16dot16_t version;

    tt_uint16_t num_glyphs;
    tt_uint16_t max_points;
    tt_uint16_t max_contours;
    tt_uint16_t max_composite_points;
    tt_uint16_t max_composite_contours;
    tt_uint16_t max_zones;
    tt_uint16_t max_twilight_points;
    tt_uint16_t max_storage;
    tt_uint16_t max_function_defs;
    tt_uint16_t max_instruction_defs;
    tt_uint16_t max_stack_elements;
    tt_uint16_t max_size_of_instructions;
    tt_uint16_t max_component_elements;
    tt_uint16_t max_component_depth;
};

} // namespace truetype
