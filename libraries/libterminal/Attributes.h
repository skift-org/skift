#pragma once

#include <libsystem/Common.h>
#include <libterminal/Color.h>

namespace Terminal
{

struct Attributes
{
    Color foreground;
    Color background;

    bool bold;
    bool invert;
    bool underline;

    static Attributes defaults()
    {
        return {FOREGROUND, BACKGROUND, false, false, false};
    }

    Attributes bolded() const
    {
        Attributes attr = *this;
        attr.bold = true;
        return attr;
    }

    Attributes inverted() const
    {
        Attributes attr = *this;
        attr.invert = true;
        return attr;
    }

    Attributes underlined() const
    {
        Attributes attr = *this;
        attr.underline = true;
        return attr;
    }

    Attributes reset() const
    {
        Attributes attr = *this;
        attr.bold = false;
        attr.invert = false;
        attr.underline = false;
        return attr;
    }

    Attributes with_forground(Color color) const
    {
        Attributes attr = *this;
        attr.foreground = color;
        return attr;
    }

    Attributes with_background(Color color) const
    {
        Attributes attr = *this;
        attr.background = color;
        return attr;
    }

    bool operator==(const Attributes &other) const
    {
        return foreground == other.foreground &&
               background == other.background &&
               bold == other.bold &&
               invert == other.invert &&
               underline == other.underline;
    }
};

} // namespace Terminal
