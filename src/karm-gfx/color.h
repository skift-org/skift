#pragma once

#include <karm-base/std.h>

namespace Karm::Gfx {

union Color {
    struct {
        uint8_t b, g, r, a;
    };

    uint32_t raw;

    constexpr Color() : b(0), g(0), r(0), a(0) {
    }

    constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : b(b), g(g), r(r), a(a) {
    }

    constexpr Color(uint32_t rgba) : raw(rgba) {}

    constexpr auto blend(const Color &bg) const -> Color {
        if (a == 0xff) {
            return *this;
        } else if (a == 0) {
            return bg;
        } else {
            uint16_t d = 255u * (bg.a + a) - bg.a * a;

            return {
                static_cast<uint8_t>((bg.r * bg.a * (255u - a) + 255u * a * r) / d),
                static_cast<uint8_t>((bg.g * bg.a * (255u - a) + 255u * a * g) / d),
                static_cast<uint8_t>((bg.b * bg.a * (255u - a) + 255u * a * b) / d),
                static_cast<uint8_t>(d / 255u),
            };
        }
    }

    constexpr auto lerp(const Color &other, float t) const -> Color {
        return {
            static_cast<uint8_t>(r + (other.r - r) * t),
            static_cast<uint8_t>(g + (other.g - g) * t),
            static_cast<uint8_t>(b + (other.b - b) * t),
            static_cast<uint8_t>(a + (other.a - a) * t),
        };
    }
};

static_assert(sizeof(Color) == 4);

} // namespace Karm::Gfx
