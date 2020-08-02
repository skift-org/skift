#pragma once

#include <libsystem/io/Stream.h>
#include <libterminal/Terminal.h>

struct TextmodeTerminalRenderer
{
    TerminalRenderer renderer;

    Stream *device;
    uint16_t *buffer;
};

Terminal *textmode_terminal_create();
