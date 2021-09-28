#pragma once

#include <libterminal/Attrs.h>
#include <libtext/Rune.h>

namespace Terminal
{

struct Cell
{
    Text::Rune rune = U' ';
    Attrs attributes;
    bool dirty;
};

} // namespace Terminal
