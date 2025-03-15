#include "color.h"

#include "karm-logger/logger.h"

namespace Vaev {

Opt<Color> parseNamedColor(Str name) {

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

Gfx::Color resolve(ColorMix const& cm, Gfx::Color currentColor) {
    Gfx::Color lhsColor = resolve(cm.lhs.color, currentColor);
    Percent lhsPerc = cm.lhs.perc.unwrapOrElse([&] {
        return Percent{100} - cm.rhs.perc.unwrapOr(Percent{50});
    });

    Gfx::Color rhsColor = resolve(cm.rhs.color, currentColor);
    Percent rhsPerc = cm.rhs.perc.unwrapOrElse([&] {
        return Percent{100} - cm.lhs.perc.unwrapOr(Percent{50});
    });

    if (lhsPerc == rhsPerc and lhsPerc == Percent{0}) {
        logWarn("cannot mix colors when both have zero percentages");
        return Gfx::WHITE;
    }

    Percent rhsPercNorm = rhsPerc;

    if (lhsPerc + rhsPerc != Percent{100})
        rhsPercNorm = rhsPercNorm / (lhsPerc + rhsPerc);
    else
        rhsPercNorm /= Percent{100};

    Gfx::Color resColor = cm.colorSpace.interpolate(lhsColor, rhsColor, rhsPercNorm.value());
    if (lhsPerc + rhsPerc < Percent{100})
        resColor = resColor.withOpacity((lhsPerc + rhsPerc).value() / 100.0);

    return resColor;
}

Gfx::Color resolve(Color const& c, Gfx::Color currentColor) {
    return c.visit(Visitor{
        [&](Gfx::Color const& srgb) {
            return srgb;
        },
        [&](Keywords::CurrentColor) {
            return currentColor;
        },
        [&](SystemColor const& system) {
            return SYSTEM_COLOR[static_cast<usize>(system)];
        },
        [&](ColorMix const& mix) {
            return resolve(mix, currentColor);
        },
    });
}

} // namespace Vaev
