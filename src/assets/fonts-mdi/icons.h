#pragma once

#include <karm-base/res.h>
#include <karm-base/string.h>

namespace Mdi {

enum struct Icon : Rune {

// NOTE: Clangd choke on this enum definition
#ifndef __clangd__
#    define ICON(id, name, code) id = code,
#    include "defs/icons.inc"
#    undef ICON
#endif
};

using enum Icon;

Slice<Rune> codepoints();

Res<Icon> byName(Str query);

Str name(Icon icon);

} // namespace Mdi
