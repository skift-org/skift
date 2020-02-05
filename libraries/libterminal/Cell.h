#pragma once

#include <libsystem/unicode/Codepoint.h>
#include <libterminal/Attributes.h>

typedef struct
{
    Codepoint codepoint;
    TerminalAttributes attributes;
    bool dirty;
} TerminalCell;