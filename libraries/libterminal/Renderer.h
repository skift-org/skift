#pragma once

#include <libterminal/Cell.h>
#include <libterminal/Cursor.h>

struct Terminal;
struct TerminalRenderer;

typedef void (*TerminalOnPaintCallback)(Terminal *terminal, TerminalRenderer *renderer, int x, int y, TerminalCell cell);
typedef void (*TerminalOnCursorCallback)(Terminal *terminal, TerminalRenderer *renderer, TerminalCursor cursor);
typedef void (*TerminalOnBlinkCallback)(Terminal *terminal, TerminalRenderer *renderer);
typedef void (*TerminalRepaintCallback)(Terminal *terminal, TerminalRenderer *renderer);
typedef void (*TerminalRendererDestroy)(TerminalRenderer *renderer);

struct TerminalRenderer
{
    TerminalOnPaintCallback on_paint;
    TerminalOnCursorCallback on_cursor;
    TerminalOnBlinkCallback on_blink;

    TerminalRepaintCallback repaint;

    TerminalRendererDestroy destroy;
};

#define TERMINAL_RENDERER(__subclass) ((TerminalRenderer *)(__subclass))

void terminal_renderer_initialize(TerminalRenderer *renderer);

void terminal_renderer_destroy(TerminalRenderer *renderer);
