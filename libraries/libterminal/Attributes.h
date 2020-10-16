#pragma once

#include <libsystem/Common.h>
#include <libterminal/Color.h>

struct TerminalAttributes
{
    TerminalColor foreground;
    TerminalColor background;

    bool bold;
    bool invert;
    bool underline;

    static TerminalAttributes defaults()
    {
        return {TERMINAL_COLOR_DEFAULT_FOREGROUND, TERMINAL_COLOR_DEFAULT_BACKGROUND, false, false, false};
    }

    TerminalAttributes bolded() const
    {
        TerminalAttributes attr = *this;
        attr.bold = true;
        return attr;
    }

    TerminalAttributes inverted() const
    {
        TerminalAttributes attr = *this;
        attr.invert = true;
        return attr;
    }

    TerminalAttributes underlined() const
    {
        TerminalAttributes attr = *this;
        attr.underline = true;
        return attr;
    }

    TerminalAttributes reset() const
    {
        TerminalAttributes attr = *this;
        attr.bold = false;
        attr.invert = false;
        attr.underline = false;
        return attr;
    }

    TerminalAttributes with_forground(TerminalColor color) const
    {
        TerminalAttributes attr = *this;
        attr.foreground = color;
        return attr;
    }

    TerminalAttributes with_background(TerminalColor color) const
    {
        TerminalAttributes attr = *this;
        attr.background = color;
        return attr;
    }

    bool operator==(const TerminalAttributes &other) const
    {
        return foreground == other.foreground &&
               background == other.background &&
               bold == other.bold &&
               invert == other.invert &&
               underline == other.underline;
    }
};
