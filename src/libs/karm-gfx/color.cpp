#include <karm-base/clamp.h>

#include "color.h"

namespace Karm::Gfx {

struct HueValueChroma {
    f64 hue;
    f64 value;
    f64 chroma;
};

static HueValueChroma _rgbToHueValueChroma(Color color) {
    f64 r = color.red / 255.0;
    f64 g = color.green / 255.0;
    f64 b = color.blue / 255.0;

    f64 rgbMax = ::max(r, g, b);
    f64 rgbMin = ::min(r, g, b);

    f64 delta = rgbMax - rgbMin;

    f64 hue = 0.0;
    if (delta != 0.0) {
        if (rgbMax == r) {
            hue = (g - b) / delta;
        } else if (rgbMax == g) {
            hue = 2.0 + (b - r) / delta;
        } else if (rgbMax == b) {
            hue = 4.0 + (r - g) / delta;
        }
    }

    hue *= 60.0;
    if (hue < 0.0) {
        hue += 360.0;
    }

    return {hue, rgbMax, delta};
}

static Color _chromaHueMinCompToRgb(f64 c, f64 h, f64 m) {
    f64 x = c * (1.0 - Math::abs(Math::fmod(h / 60.0, 2.0) - 1.0));

    f64 r = 0.0;
    f64 g = 0.0;
    f64 b = 0.0;

    if (h < 60.0) {
        r = c;
        g = x;
    } else if (h < 120.0) {
        r = x;
        g = c;
    } else if (h < 180.0) {
        g = c;
        b = x;
    } else if (h < 240.0) {
        g = x;
        b = c;
    } else if (h < 300.0) {
        r = x;
        b = c;
    } else {
        r = c;
        b = x;
    }

    return {
        static_cast<u8>(Math::round((r + m) * 255.0)),
        static_cast<u8>(Math::round((g + m) * 255.0)),
        static_cast<u8>(Math::round((b + m) * 255.0)),
    };
}

Hsv rgbToHsv(Color color) {
    auto [hue, value, chroma] = _rgbToHueValueChroma(color);

    f64 saturation = value == 0.0f ? 0.0f : chroma / value;

    return {hue, saturation, value};
}

Hsl rgbToHsl(Color color) {
    auto [hue, value, chroma] = _rgbToHueValueChroma(color);

    f64 lightness = value - chroma / 2.0f;
    f64 saturation =
        lightness == 0.0f or lightness == 1.0f
            ? 0.0f
            : (value - lightness) / ::min(lightness, 1.0f - lightness);

    return {hue, saturation, lightness};
}

Color hsvToRgb(Hsv hsv) {
    f64 h = hsv.hue;
    f64 s = hsv.saturation;
    f64 v = hsv.value;

    f64 c = v * s;
    f64 minComp = v - c;

    return _chromaHueMinCompToRgb(c, h, minComp).withOpacity(1);
}

Color hslToRgb(Hsl hsl) {
    f64 h = hsl.hue;
    f64 s = hsl.saturation;
    f64 l = hsl.lightness;

    f64 c = (1.0f - Math::abs(2.0 * l - 1.0)) * s;
    f64 minComp = l - c / 2.0;

    return _chromaHueMinCompToRgb(c, h, minComp);
}

} // namespace Karm::Gfx
