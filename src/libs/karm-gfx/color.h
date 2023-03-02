#pragma once

#include <karm-base/std.h>
#include <karm-math/vec.h>

namespace Karm::Gfx {

struct Color {
    u8 red, green, blue, alpha;

    static constexpr Color fromHex(u32 hex) {
        return {
            static_cast<u8>((hex >> 16) & 0xFF),
            static_cast<u8>((hex >> 8) & 0xFF),
            static_cast<u8>(hex & 0xFF),
            static_cast<u8>(0xFF),
        };
    }

    static constexpr Color fromRgb(u8 red, u8 green, u8 blue) {
        return {red, green, blue, 255};
    }

    static constexpr Color fromRgba(u8 red, u8 green, u8 blue, u8 alpha) {
        return {red, green, blue, alpha};
    }

    constexpr Color() : red(0), green(0), blue(0), alpha(0) {}

    constexpr Color(u8 red, u8 green, u8 blue, u8 alpha = 255) : red(red), green(green), blue(blue), alpha(alpha) {}

    constexpr Color(Math::Vec4u v) : red(v.x), green(v.y), blue(v.z), alpha(v.w) {}

    constexpr Color blendOver(Color const background) const {
        if (alpha == 0xff) {
            return *this;
        } else if (alpha == 0) {
            return background;
        } else if (background.alpha == 255u) {
            return {
                static_cast<u8>((background.red * 255u * (255u - alpha) + 255u * alpha * red) / 65025),
                static_cast<u8>((background.green * 255u * (255u - alpha) + 255u * alpha * green) / 65025),
                static_cast<u8>((background.blue * 255u * (255u - alpha) + 255u * alpha * blue) / 65025),
                static_cast<u8>(255),
            };
        } else {
            u16 d = 255u * (background.alpha + alpha) - background.alpha * alpha;

            return {
                static_cast<u8>((background.red * background.alpha * (255u - alpha) + 255u * alpha * red) / d),
                static_cast<u8>((background.green * background.alpha * (255u - alpha) + 255u * alpha * green) / d),
                static_cast<u8>((background.blue * background.alpha * (255u - alpha) + 255u * alpha * blue) / d),
                static_cast<u8>(d / 255u),
            };
        }
    }

    constexpr Color lerpWith(Color const other, f64 const t) const {
        return {
            static_cast<u8>(red + (other.red - red) * t),
            static_cast<u8>(green + (other.green - green) * t),
            static_cast<u8>(blue + (other.blue - blue) * t),
            static_cast<u8>(alpha + (other.alpha - alpha) * t),
        };
    }

    constexpr Color withOpacity(f64 const opacity) const {
        return {
            static_cast<u8>(red),
            static_cast<u8>(green),
            static_cast<u8>(blue),
            static_cast<u8>(alpha * opacity),
        };
    }

    constexpr operator Math::Vec4u() const {
        return {
            static_cast<u32>(red),
            static_cast<u32>(green),
            static_cast<u32>(blue),
            static_cast<u32>(alpha),
        };
    }

    constexpr f64 luminance() const {
        auto r = this->red / 255.0;
        auto g = this->green / 255.0;
        auto b = this->blue / 255.0;
        return sqrt(0.299 * r * r + 0.587 * g * g + 0.114 * b * b);
    }
};

struct Hsv {
    f64 hue, saturation, value;

    Ordr cmp(Hsv const &other) const {
        return hue == other.hue &&
                       saturation == other.saturation and value == other.value
                   ? Ordr::EQUAL
                   : Ordr::LESS;
    }
};

Hsv rgbToHsv(Color color);

Color hsvToRgb(Hsv hsv);

} // namespace Karm::Gfx
