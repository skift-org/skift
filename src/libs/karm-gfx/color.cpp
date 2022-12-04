#include "color.h"

namespace Karm::Gfx {

Hsv rgbToHsv(Color color) {
    float r = color.red / 255.0f;
    float g = color.green / 255.0f;
    float b = color.blue / 255.0f;

    float max = std::max(r, std::max(g, b));
    float min = std::min(r, std::min(g, b));

    float delta = max - min;

    float hue = 0.0f;
    if (delta != 0.0f) {
        if (max == r) {
            hue = (g - b) / delta;
        } else if (max == g) {
            hue = 2.0f + (b - r) / delta;
        } else if (max == b) {
            hue = 4.0f + (r - g) / delta;
        }
    }

    hue *= 60.0f;
    if (hue < 0.0f) {
        hue += 360.0f;
    }

    float saturation = max == 0.0f ? 0.0f : delta / max;

    return {hue, saturation, max};
}

Color hsvToRgb(Hsv hsv) {
    float h = hsv.hue;
    float s = hsv.saturation;
    float v = hsv.value;

    float c = v * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;

    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

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
    } else if (h < 360.0f) {
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
