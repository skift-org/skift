#include "terminal/FramebufferTerminal.h"
#include "terminal/Common.h"

void framebuffer_terminal_repaint(Terminal *terminal, FramebufferTerminalRenderer *renderer)
{
    Painter &painter = renderer->framebuffer->painter();

    for (int y = 0; y < terminal->height; y++)
    {
        for (int x = 0; x < terminal->width; x++)
        {
            TerminalCell cell = terminal_cell_at(terminal, x, y);

            if (cell.dirty)
            {
                terminal::render_cell(painter, x, y, cell);

                renderer->framebuffer->mark_dirty(terminal::cell_bound(x, y));

                terminal_cell_undirty(terminal, x, y);
            }
        }
    }

    renderer->framebuffer->blit();
}

void framebuffer_terminal_render_cursor(Terminal *terminal, FramebufferTerminalRenderer *renderer, int x, int y, bool visible)
{
    Painter &painter = renderer->framebuffer->painter();

    TerminalCell cell = terminal_cell_at(terminal, x, y);

    if (visible)
    {
        cell.attributes.inverted = true;
        cell.attributes.foreground = TERMINAL_COLOR_YELLOW;
    }

    terminal::render_cell(painter, x, y, cell);

    renderer->framebuffer->mark_dirty(terminal::cell_bound(x, y));
}

void framebuffer_terminal_on_cursor(Terminal *terminal, FramebufferTerminalRenderer *renderer, TerminalCursor cursor)
{
    __unused(terminal);

    framebuffer_terminal_render_cursor(terminal, renderer, renderer->framebuffer_cursor.x(), renderer->framebuffer_cursor.y(), false);

    renderer->framebuffer_cursor = Vec2i(cursor.x, cursor.y);

    framebuffer_terminal_render_cursor(terminal, renderer, renderer->framebuffer_cursor.x(), renderer->framebuffer_cursor.y(), true);
    renderer->cursor_blink = false;
}

void framebuffer_terminal_on_blink(Terminal *terminal, FramebufferTerminalRenderer *renderer)
{
    renderer->cursor_blink = !renderer->cursor_blink;

    framebuffer_terminal_render_cursor(terminal, renderer, renderer->framebuffer_cursor.x(), renderer->framebuffer_cursor.y(), renderer->cursor_blink);

    renderer->framebuffer->blit();
}

void framebuffer_terminal_destroy(FramebufferTerminalRenderer *renderer)
{
    renderer->framebuffer = nullptr;
}

Terminal *framebuffer_terminal_create()
{
    auto framebuffer = Framebuffer::open();

    if (!framebuffer.success())
    {
        return nullptr;
    }

    FramebufferTerminalRenderer *renderer = __create(FramebufferTerminalRenderer);

    TERMINAL_RENDERER(renderer)->on_cursor = (TerminalOnCursorCallback)framebuffer_terminal_on_cursor;
    TERMINAL_RENDERER(renderer)->repaint = (TerminalRepaintCallback)framebuffer_terminal_repaint;
    TERMINAL_RENDERER(renderer)->destroy = (TerminalRendererDestroy)framebuffer_terminal_destroy;
    TERMINAL_RENDERER(renderer)->on_blink = (TerminalOnBlinkCallback)framebuffer_terminal_on_blink;

    renderer->framebuffer = framebuffer.take_value();

    return terminal_create(
        renderer->framebuffer->resolution().width() / terminal::cell_size().x(),
        renderer->framebuffer->resolution().height() / terminal::cell_size().y(),
        TERMINAL_RENDERER(renderer));
}
