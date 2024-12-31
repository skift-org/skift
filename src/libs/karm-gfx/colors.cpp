#include "colors.h"

namespace Karm::Gfx {

Color randomColor(Math::Rand &rand) {
    return COLORS[rand.nextInt(COLORS.len())];
}

Color randomColor() {
    static Math::Rand rand{123};
    return randomColor(rand);
}

Gfx::Color rainbowColor() {
    static f64 hue = 0;
    auto res = Gfx::hsvToRgb({hue, 1, 1});
    hue += 1;
    if (hue > 360)
        hue = 0;
    return res;
}

} // namespace Karm::Gfx
