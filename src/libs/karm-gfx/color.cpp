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

    f64 hue = 0.0f;
    if (delta != 0.0f) {
        if (rgbMax == r) {
            hue = (g - b) / delta;
        } else if (rgbMax == g) {
            hue = 2.0f + (b - r) / delta;
        } else if (rgbMax == b) {
            hue = 4.0f + (r - g) / delta;
        }
    }

    hue *= 60.0f;
    if (hue < 0.0f) {
        hue += 360.0f;
    }

    return {hue, rgbMax, delta};
}

static Color _chromaHueMinCompToRgb(f64 c, f64 h, f64 m) {
    f64 x = c * (1.0f - Math::abs(fmod(h / 60.0f, 2.0f) - 1.0f));

    f64 r = 0.0f;
    f64 g = 0.0f;
    f64 b = 0.0f;

    if (h < 60.0f) {
        r = c;
        g = x;
    } else if (h < 120.0f) {
        r = x;
        g = c;
    } else if (h < 180.0f) {
        g = c;
        b = x;
    } else if (h < 240.0f) {
        g = x;
        b = c;
    } else if (h < 300.0f) {
        r = x;
        b = c;
    } else {
        r = c;
        b = x;
    }

    return {
        static_cast<u8>(round((r + m) * 255.0f)),
        static_cast<u8>(round((g + m) * 255.0f)),
        static_cast<u8>(round((b + m) * 255.0f)),
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

    f64 c = (1.0f - Math::abs(2.0f * l - 1.0f)) * s;
    f64 minComp = l - c / 2.0f;

    return _chromaHueMinCompToRgb(c, h, minComp);
}

} // namespace Karm::Gfx
