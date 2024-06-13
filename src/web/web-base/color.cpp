#include "color.h"

namespace Web {

Opt<Color> parseNamedColor(Str name) {
    if (name == "transparent")
        return TRANSPARENT;

    if (name == "currentColor")
        return Color::CURRENT;

#define COLOR(NAME, _, VALUE) \
    if (name == #NAME)        \
        return NAME;
#include "defs/colors.inc"
#undef COLOR

    return NONE;
}

} // namespace Web
