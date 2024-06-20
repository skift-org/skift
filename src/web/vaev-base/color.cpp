#include "color.h"

namespace Vaev {

Opt<Color> parseNamedColor(Str name) {
    if (name == "transparent")
        return TRANSPARENT;

    if (name == "currentColor")
        return Color::CURRENT;

#define COLOR(ID, NAME, ...) \
    if (name == #NAME)       \
        return ID;
#include "defs/colors.inc"
#undef COLOR

    return NONE;
}

Opt<SystemColor> parseSystemColor(Str name) {
#define COLOR(ID, NAME, ...) \
    if (name == #NAME)       \
        return SystemColor::ID;
#include "defs/system-colors.inc"
#undef SYSTEM_COLOR

    return NONE;
}

} // namespace Vaev
