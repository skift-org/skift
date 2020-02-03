/* Copyright © 2018-2020 N. Van Bossuyt.                                      */
/* This code is licensed under the MIT License.                               */
/* See: LICENSE.md                                                            */

#include <libterminal/Attributes.h>

TerminalAttributes terminal_attributes_bold(TerminalAttributes attributes)
{
    attributes.bold = true;

    return attributes;
}

TerminalAttributes terminal_attributes_inverted(TerminalAttributes attributes)
{
    attributes.inverted = true;

    return attributes;
}

TerminalAttributes terminal_attributes_underline(TerminalAttributes attributes)
{
    attributes.underline = true;

    return attributes;
}

TerminalAttributes terminal_attributes_regular(TerminalAttributes attributes)
{
    attributes.bold = false;
    attributes.inverted = false;
    attributes.underline = false;

    return attributes;
}

TerminalAttributes terminal_attributes_with_foreground(TerminalAttributes attributes, TerminalColor foreground)
{
    attributes.foreground = foreground;

    return attributes;
}

TerminalAttributes terminal_attributes_with_background(TerminalAttributes attributes, TerminalColor background)
{
    attributes.background = background;

    return attributes;
}

bool terminal_attributes_equals(TerminalAttributes left, TerminalAttributes right)
{
    return left.foreground == right.foreground &&
           left.background == right.background &&
           left.bold == right.bold &&
           left.inverted == right.inverted &&
           left.underline == right.underline;
}
