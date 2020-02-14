#pragma once

#include <libgraphic/framebuffer.h>
#include <libterminal/Terminal.h>

typedef struct
{
    TerminalRenderer renderer;

    Font *mono_font;
    Point framebuffer_cursor;
    framebuffer_t *framebuffer;
    bool cursor_blink;
} FramebufferTerminalRenderer;

Terminal *framebuffer_terminal_create(void);