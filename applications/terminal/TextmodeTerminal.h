#pragma once

#include <libdevice/textmode.h>
#include <libsystem/io/Stream.h>
#include <libterminal/Terminal.h>

typedef struct
{
    TerminalRenderer renderer;

    Stream *device;
    textmode_info_t info;
    ushort *buffer;
} TextmodeTerminalRenderer;

Terminal *textmode_terminal_create(void);