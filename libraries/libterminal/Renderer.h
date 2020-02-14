#pragma once

#include <libterminal/Cell.h>
#include <libterminal/Cursor.h>

struct Terminal;
struct TerminalRenderer;

typedef void (*TerminalOnPaintCallback)(struct Terminal *terminal, struct TerminalRenderer *renderer, int x, int y, TerminalCell cell);
typedef void (*TerminalOnCursorCallback)(struct Terminal *terminal, struct TerminalRenderer *renderer, TerminalCursor cursor);
typedef void (*TerminalOnBlinkCallback)(struct Terminal *terminal, struct TerminalRenderer *renderer);
typedef void (*TerminalRepaintCallback)(struct Terminal *terminal, struct TerminalRenderer *renderer);
typedef void (*TerminalRendererDestroy)(struct TerminalRenderer *renderer);

typedef struct TerminalRenderer
{
    TerminalOnPaintCallback on_paint;
    TerminalOnCursorCallback on_cursor;
    TerminalOnBlinkCallback on_blink;

    TerminalRepaintCallback repaint;

    TerminalRendererDestroy destroy;
} TerminalRenderer;

#define TERMINAL_RENDERER(__subclass) ((TerminalRenderer *)(__subclass))

void terminal_renderer_initialize(TerminalRenderer *renderer);

void terminal_renderer_destroy(TerminalRenderer *renderer);
