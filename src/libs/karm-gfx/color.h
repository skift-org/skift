#pragma once

#include <karm-io/fmt.h>
#include <karm-math/vec.h>

namespace Karm::Gfx {

struct Color {
    u8 red, green, blue, alpha;

    always_inline static constexpr Color fromHex(u32 hex) {
        return {
            static_cast<u8>((hex >> 16) & 0xFF),
            static_cast<u8>((hex >> 8) & 0xFF),
            static_cast<u8>(hex & 0xFF),
            static_cast<u8>(0xFF),
        };
    }

    always_inline static constexpr Color fromRgb(u8 red, u8 green, u8 blue) {
        return {red, green, blue, 255};
    }

    always_inline static constexpr Color fromRgba(u8 red, u8 green, u8 blue, u8 alpha) {
        return {red, green, blue, alpha};
    }

    always_inline constexpr Color()
        : red(0), green(0), blue(0), alpha(0) {}

    always_inline constexpr Color(u8 red, u8 green, u8 blue, u8 alpha = 255)
        : red(red), green(green), blue(blue), alpha(alpha) {}

    always_inline constexpr Color(Math::Vec4u v)
        : red(v.x), green(v.y), blue(v.z), alpha(v.w) {}

    always_inline constexpr Color blendOver(Color const background) const {
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

    static constexpr auto RED_COMPONENT = [](auto &c) -> auto & {
        return c.red;
    };
    static constexpr auto GREEN_COMPONENT = [](auto &c) -> auto & {
        return c.green;
    };
    static constexpr auto BLUE_COMPONENT = [](auto &c) -> auto & {
        return c.blue;
    };

    always_inline constexpr Color blendOverComponent(Color const background, auto comp) const {
        if (alpha == 0xff) {
            Color res = background;
            comp(res) = comp(*this);
            res.alpha = 255;
            return res;
        } else if (alpha == 0) {
            return background;
        } else if (background.alpha == 255u) {
            Color res = background;
            comp(res) = static_cast<u8>((comp(background) * 255u * (255u - alpha) + 255u * alpha * comp(*this)) / 65025);
            res.alpha = 255;
            return res;
        } else {
            Color res = background;
            u16 d = 255u * (background.alpha + alpha) - background.alpha * alpha;
            comp(res) = static_cast<u8>((comp(background) * background.alpha * (255u - alpha) + 255u * alpha * comp(*this)) / d);
            res.alpha = d / 255u;
            return res;
        }
    }

    always_inline constexpr Color lerpWith(Color const other, f64 const t) const {
        return {
            static_cast<u8>(red + (other.red - red) * t),
            static_cast<u8>(green + (other.green - green) * t),
            static_cast<u8>(blue + (other.blue - blue) * t),
            static_cast<u8>(alpha + (other.alpha - alpha) * t),
        };
    }

    always_inline constexpr Color withOpacity(f64 const opacity) const {
        return {
            static_cast<u8>(red),
            static_cast<u8>(green),
            static_cast<u8>(blue),
            static_cast<u8>(alpha * opacity),
        };
    }

    always_inline constexpr operator Math::Vec4u() const {
        return {
            static_cast<u32>(red),
            static_cast<u32>(green),
            static_cast<u32>(blue),
            static_cast<u32>(alpha),
        };
    }

    always_inline constexpr f64 luminance() const {
        auto r = red / 255.0;
        auto g = green / 255.0;
        auto b = blue / 255.0;
        return Math::sqrt(0.299 * r * r + 0.587 * g * g + 0.114 * b * b);
    }

    always_inline constexpr Color sample(Math::Vec2f) const {
        return *this;
    }
};

struct Hsv {
    f64 hue, saturation, value;

    Hsv withHue(f64 hue) const {
        return {hue, saturation, value};
    }

    Hsv withSaturation(f64 saturation) const {
        return {hue, saturation, value};
    }

    Hsv withValue(f64 value) const {
        return {hue, saturation, value};
    }

    auto operator<=>(Hsv const &other) const = default;
};

Hsv rgbToHsv(Color color);

Color hsvToRgb(Hsv hsv);

struct YCbCr {
    f32 y, cb, cr;

    auto operator<=>(YCbCr const &other) const = default;
};

static inline YCbCr rgbToYCbCr(Color color) {
    YCbCr yCbCr;
    yCbCr.y = 0.299 * color.red + 0.587 * color.green + 0.114 * color.blue;
    yCbCr.cb = 128 - 0.168736 * color.red - 0.331264 * color.green + 0.5 * color.blue;
    yCbCr.cr = 128 + 0.5 * color.red - 0.418688 * color.green - 0.081312 * color.blue;
    return yCbCr;
}

static inline Color yCbCrToRgb(YCbCr yCbCr) {
    float r = yCbCr.y + 1.402f * yCbCr.cr + 128;
    float g = yCbCr.y - 0.344f * yCbCr.cb - 0.714f * yCbCr.cr + 128;
    float b = yCbCr.y + 1.772f * yCbCr.cb + 128;

    r = clamp(r, 0.0f, 255.0f);
    g = clamp(g, 0.0f, 255.0f);
    b = clamp(b, 0.0f, 255.0f);
    return Gfx::Color::fromRgba(r, g, b, 255);
}

} // namespace Karm::Gfx

Reflectable$(Gfx::Color, red, green, blue, alpha);
