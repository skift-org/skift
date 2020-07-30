#pragma once

#include <libgraphic/Framebuffer.h>
#include <libterminal/Terminal.h>

typedef struct
{
    TerminalRenderer renderer;

    RefPtr<Font> mono_font;
    Vec2i framebuffer_cursor;
    OwnPtr<Framebuffer> framebuffer;
    bool cursor_blink;
} FramebufferTerminalRenderer;

Terminal *framebuffer_terminal_create(void);
