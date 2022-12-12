#include <karm-base/clamp.h>

#include "color.h"

namespace Karm::Gfx {

Hsv rgbToHsv(Color color) {
    double r = color.red / 255.0;
    double g = color.green / 255.0;
    double b = color.blue / 255.0;

    double rgbMax = Karm::max(r, g, b);
    double rgbMin = Karm::min(r, g, b);

    double delta = rgbMax - rgbMin;

    double hue = 0.0f;
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

    double saturation = rgbMax == 0.0f ? 0.0f : delta / rgbMax;

    return {hue, saturation, rgbMax};
}

Color hsvToRgb(Hsv hsv) {
    double h = hsv.hue;
    double s = hsv.saturation;
    double v = hsv.value;

    double c = v * s;
    double x = c * (1.0f - abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    double m = v - c;

    double r = 0.0f;
    double g = 0.0f;
    double b = 0.0f;

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
        static_cast<uint8_t>((r + m) * 255.0f),
        static_cast<uint8_t>((g + m) * 255.0f),
        static_cast<uint8_t>((b + m) * 255.0f),
        255,
    };
}

} // namespace Karm::Gfx
