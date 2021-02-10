#pragma once

#include <libutils/unicode/Codepoint.h>
#include <libterminal/Attributes.h>

namespace terminal
{
struct Cell
{
    Codepoint codepoint;
    Attributes attributes;
    bool dirty;
};

} // namespace terminal
