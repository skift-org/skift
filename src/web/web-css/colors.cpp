#include "colors.h"

namespace Web::Css {

Opt<Color> parseNamedColor(Str name) {
    if (name == "transparent")
        return TRANSPARENT;

    if (name == "currentColor")
        return CURRENT_COLOR;

#define COLOR(NAME, _, VALUE) \
    if (name == #NAME)        \
        return NAME;
#include "defs/colors.inc"
#undef COLOR

    return NONE;
}

} // namespace Web::Css
