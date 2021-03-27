#pragma once

#include <libgraphic/ColorsNames.h>
#include <libsystem/Common.h>
#include <libsystem/math/Lerp.h>
#include <libutils/String.h>

namespace Graphic
{


struct Color
{
private:
    uint8_t _red;
    uint8_t _green;
    uint8_t _blue;
    uint8_t _alpha;

    constexpr Color(uint8_t red, uint8_t green, uint8_t blue)
        : _red(red),
          _green(green),
          _blue(blue),
          _alpha(255)
    {
    }

    constexpr Color(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
        : _red(red),
          _green(green),
          _blue(blue),
          _alpha(alpha)
    {
    }

public:
    constexpr Color() : Color(0, 0, 0) {}

    constexpr uint8_t red() const { return _red; }

    constexpr uint8_t green() const { return _green; }

    constexpr uint8_t blue() const { return _blue; }

    constexpr uint8_t alpha() const { return _alpha; }

    constexpr float redf() const { return _red / 255.0; }

    constexpr float greenf() const { return _green / 255.0; }

    constexpr float bluef() const { return _blue / 255.0; }

    constexpr float alphaf() const { return _alpha / 255.0; }

    static constexpr Color from_rgba_byte(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha)
    {
        return {red, green, blue, alpha};
    }

    static constexpr Color from_rgb_byte(uint8_t red, uint8_t green, uint8_t blue)
    {
        return {red, green, blue, 0xff};
    }

    static constexpr Color from_monochrome_byte(uint8_t grey)
    {
        return {grey, grey, grey, 0xff};
    }

    static constexpr Color from_monochrome_alpha_byte(uint8_t grey, uint8_t alpha)
    {
        return {grey, grey, grey, alpha};
    }

    static constexpr Color from_hex(uint32_t hex)
    {
        return {
            static_cast<uint8_t>((hex >> 16) & 0xff),
            static_cast<uint8_t>((hex >> 8) & 0xff),
            static_cast<uint8_t>((hex & 0xff)),
            0xff,
        };
    }

    static constexpr Color from_hexa(uint32_t hex)
    {
        return {
            static_cast<uint8_t>((hex >> 24) & 0xff),
            static_cast<uint8_t>((hex >> 16) & 0xff),
            static_cast<uint8_t>((hex >> 8) & 0xff),
            static_cast<uint8_t>((hex & 0xff)),
        };
    }

    static constexpr Color from_hsv(double hue, double saturation, double value)
    {
        return from_hsva(hue, saturation, value, 1.0);
    }

    static constexpr Color from_hsva(double hue, double saturation, double value, double alpha)
    {
        if (saturation == 0)
        {
            return from_rgba(value, value, value, alpha);
        }

        if (hue == 360)
            hue = 0;
        else
            hue = hue / 60;

        int i = (int)hue;

        float f = hue - i;
        float p = value * (1.0 - saturation);
        float q = value * (1.0 - (saturation * f));
        float t = value * (1.0 - (saturation * (1.0 - f)));

        switch (i)
        {
        case 0:
            return from_rgba(value, t, p, alpha);

        case 1:
            return from_rgba(q, value, p, alpha);

        case 2:
            return from_rgba(p, value, t, alpha);

        case 3:
            return from_rgba(p, q, value, alpha);

        case 4:
            return from_rgba(t, p, value, alpha);

        default:
            return from_rgba(value, p, q, alpha);
        }
    }

    static constexpr Color from_rgb(double red, double green, double blue)
    {
        return from_rgba(red, green, blue, 1.0);
    }

    static constexpr Color from_rgba(double red, double green, double blue, double alpha)
    {
        return {
            static_cast<uint8_t>(red * 0xff),
            static_cast<uint8_t>(green * 0xff),
            static_cast<uint8_t>(blue * 0xff),
            static_cast<uint8_t>(alpha * 0xff),
        };
    }

    static Color parse(String string);

    static constexpr Color blend(Color fg, Color gb)
    {
        float a = (1 - fg.alphaf()) * gb.alphaf() + fg.alphaf();
        float r = ((1 - fg.alphaf()) * gb.alphaf() * gb.redf() + fg.alphaf() * fg.redf()) / a;
        float g = ((1 - fg.alphaf()) * gb.alphaf() * gb.greenf() + fg.alphaf() * fg.greenf()) / a;
        float b = ((1 - fg.alphaf()) * gb.alphaf() * gb.bluef() + fg.alphaf() * fg.bluef()) / a;

        return from_rgba(r, g, b, a);
    }

    static constexpr Color lerp(Color from, Color to, float transition)
    {
        return from_rgba(
            ::lerp(from.redf(), to.redf(), transition),
            ::lerp(from.greenf(), to.greenf(), transition),
            ::lerp(from.bluef(), to.bluef(), transition),
            ::lerp(from.alphaf(), to.alphaf(), transition));
    }

    static constexpr Color blerp(Color c00, Color c10, Color c01, Color c11, float transitionx, float transitiony)
    {
        return lerp(lerp(c00, c10, transitionx),
                    lerp(c01, c11, transitionx), transitiony);
    }

    constexpr Color with_alpha(float alpha) const
    {
        return {
            red(),
            green(),
            blue(),
            static_cast<uint8_t>(0xff * alpha),
        };
    }

    constexpr Color with_alpha_byte(uint8_t alpha) const
    {
        return {
            red(),
            green(),
            blue(),
            alpha,
        };
    }

    constexpr bool operator==(const Color &other) const
    {
        return red() == other.red() &&
               green() == other.green() &&
               blue() == other.blue() &&
               alpha() == other.alpha();
    }
};

class Colors
{
private:
    Colors(/* args */) {}
    ~Colors() {}
    /* data */
public:
#define __ENTRY(__name, __value) static constexpr Color __name = Color::from_hexa(__value);
    COLOR_LIST(__ENTRY)
#undef __ENTRY
};

} // namespace Graphic
