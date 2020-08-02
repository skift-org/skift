#pragma once

#include <libsystem/unicode/Codepoint.h>
#include <libterminal/Attributes.h>

struct TerminalCell
{
    Codepoint codepoint;
    TerminalAttributes attributes;
    bool dirty;
};
