#pragma once

#include <libterminal/Attributes.h>
#include <libutils/unicode/Codepoint.h>

namespace Terminal
{
struct Cell
{
    Codepoint codepoint;
    Attributes attributes;
    bool dirty;
};

} // namespace Terminal
