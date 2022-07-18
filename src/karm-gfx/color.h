#pragma once

#include <karm-base/std.h>

namespace Karm::Gfx {

struct Color {
    uint8_t red, green, blue, alpha;

    static constexpr Color fromHex(uint32_t hex) {
        return {
            static_cast<uint8_t>((hex >> 16) & 0xFF),
            static_cast<uint8_t>((hex >> 8) & 0xFF),
            static_cast<uint8_t>(hex & 0xFF),
            static_cast<uint8_t>(0xFF),
        };
    }

    static constexpr Color fromRgb(uint8_t red, uint8_t green, uint8_t blue) {
        return {red, green, blue, 255};
    }

    static constexpr Color fromRgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
        return {red, green, blue, alpha};
    }

    constexpr Color() : red(0), green(0), blue(0), alpha(0) {}

    constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255) : red(red), green(green), blue(blue), alpha(alpha) {}

    // (b * 255u * (255u - a) + 255u * a * f) / 65025
    // (b * (255² - 255*a) + 255u * a * f) / 65025
    // (b * (255² - 255*a) + 255u * a * f) / 65025
    // (65025b - 255ab + 255af) / 65025

    constexpr Color blendOver(Color const background) const {
        if (alpha == 0xff) {
            return *this;
        } else if (alpha == 0) {
            return background;
        } else if (background.alpha == 255u) {
            return {
                static_cast<uint8_t>((background.red * 255u * (255u - alpha) + 255u * alpha * red) / 65025),
                static_cast<uint8_t>((background.green * 255u * (255u - alpha) + 255u * alpha * green) / 65025),
                static_cast<uint8_t>((background.blue * 255u * (255u - alpha) + 255u * alpha * blue) / 65025),
                static_cast<uint8_t>(255),
            };
        } else {
            uint16_t d = 255u * (background.alpha + alpha) - background.alpha * alpha;

            return {
                static_cast<uint8_t>((background.red * background.alpha * (255u - alpha) + 255u * alpha * red) / d),
                static_cast<uint8_t>((background.green * background.alpha * (255u - alpha) + 255u * alpha * green) / d),
                static_cast<uint8_t>((background.blue * background.alpha * (255u - alpha) + 255u * alpha * blue) / d),
                static_cast<uint8_t>(d / 255u),
            };
        }
    }

    constexpr Color lerpWith(Color const other, double const t) const {
        return {
            static_cast<uint8_t>(red + (other.red - red) * t),
            static_cast<uint8_t>(green + (other.green - green) * t),
            static_cast<uint8_t>(blue + (other.blue - blue) * t),
            static_cast<uint8_t>(alpha + (other.alpha - alpha) * t),
        };
    }

    constexpr Color withOpacity(double const opacity) const {
        return {
            static_cast<uint8_t>(red),
            static_cast<uint8_t>(green),
            static_cast<uint8_t>(blue),
            static_cast<uint8_t>(alpha * opacity),
        };
    }
};

} // namespace Karm::Gfx
