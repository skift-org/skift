#pragma once

#include <karm-gfx/colors.h>

// CSS Color Module Level 3
// https://www.w3.org/TR/css-color-3/#html4

namespace Web::Css {

#define COLOR(NAME, _, VALUE) \
    inline constexpr Gfx::Color NAME = Gfx::Color::fromHex(VALUE);
#include "defs/colors.inc"
#undef COLOR

// 4.4. ‘currentColor’ color keyword

struct CurrentColor {
    auto operator<=>(CurrentColor const &) const = default;
};

constexpr inline auto CURRENT_COLOR = CurrentColor{};

using Color = Var<CurrentColor, Gfx::Color>;

} // namespace Web::Css
