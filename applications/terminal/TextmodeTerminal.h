#pragma once

#include <libsystem/io/Stream.h>
#include <libterminal/Terminal.h>

typedef struct
{
    TerminalRenderer renderer;

    Stream *device;
    uint16_t *buffer;
} TextmodeTerminalRenderer;

Terminal *textmode_terminal_create(void);
