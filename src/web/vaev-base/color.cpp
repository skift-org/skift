#include "color.h"

namespace Vaev {

Opt<Color> parseNamedColor(Str name) {
    if (eqCi(name, "transparent"s))
        return TRANSPARENT;

    if (eqCi(name, "currentColor"s))
        return Color::CURRENT;

#define COLOR(ID, NAME, ...)   \
    if (eqCi(name, #NAME ""s)) \
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
#undef COLOR

    return NONE;
}

static Array<Gfx::Color, static_cast<usize>(SystemColor::_LEN)> SYSTEM_COLOR = {
#define COLOR(NAME, _, VALUE) Gfx::Color::fromHex(VALUE),
#include "defs/system-colors.inc"
#undef COLOR
};

Gfx::Color resolve(Color c, Gfx::Color currentColor) {
    switch (c.type) {
    case Color::Type::SRGB:
        return c.srgb;

    case Color::Type::SYSTEM:
        return SYSTEM_COLOR[static_cast<usize>(c.system)];

    case Color::Type::CURRENT:
        return currentColor;

    default:
        panic("Invalid color type");
    }
}

} // namespace Vaev
