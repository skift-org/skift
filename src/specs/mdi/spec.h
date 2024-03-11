#pragma once

#include <karm-base/res.h>
#include <karm-base/string.h>

namespace Mdi {

enum struct Icon {
#define ICON(id, name, code) id = code,
#include "defs/icons.inc"
#undef ICON
};

using enum Icon;

Slice<Rune> codepoints();

Res<Icon> byName(Str query);

Str name(Icon icon);

} // namespace Mdi
