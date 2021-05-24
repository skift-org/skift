#pragma once

#include <libterminal/Attributes.h>
#include <libtext/Rune.h>

namespace Terminal
{

struct Cell
{
    Text::Rune rune = U' ';
    Attributes attributes;
    bool dirty;
};

} // namespace Terminal
