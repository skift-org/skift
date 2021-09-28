#pragma once

#include <libterminal/Color.h>
#include <libutils/Prelude.h>

namespace Terminal
{

struct Attrs
{
    Color foreground = FOREGROUND;
    Color background = BACKGROUND;

    bool bold = false;
    bool invert = false;
    bool underline = false;

    Attrs bolded() const
    {
        Attrs attr = *this;
        attr.bold = true;
        return attr;
    }

    Attrs inverted() const
    {
        Attrs attr = *this;
        attr.invert = true;
        return attr;
    }

    Attrs underlined() const
    {
        Attrs attr = *this;
        attr.underline = true;
        return attr;
    }

    Attrs reset() const
    {
        Attrs attr = *this;
        attr.bold = false;
        attr.invert = false;
        attr.underline = false;
        return attr;
    }

    Attrs with_forground(Color color) const
    {
        Attrs attr = *this;
        attr.foreground = color;
        return attr;
    }

    Attrs with_background(Color color) const
    {
        Attrs attr = *this;
        attr.background = color;
        return attr;
    }

    bool operator==(const Attrs &other) const
    {
        return foreground == other.foreground &&
               background == other.background &&
               bold == other.bold &&
               invert == other.invert &&
               underline == other.underline;
    }
};

} // namespace Terminal
