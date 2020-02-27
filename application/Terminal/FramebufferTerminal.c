#include "Terminal/FramebufferTerminal.h"

static Point char_size = (Point){7, 16};

static Color framebuffer_colors[__TERMINAL_COLOR_COUNT] = {
    [TERMINAL_COLOR_BLACK] = COLOR(0x000000),
    [TERMINAL_COLOR_RED] = COLOR(0xff3333),
    [TERMINAL_COLOR_GREEN] = COLOR(0xb8cc52),
    [TERMINAL_COLOR_YELLOW] = COLOR(0xe6c446),
    [TERMINAL_COLOR_BLUE] = COLOR(0x36a3d9),
    [TERMINAL_COLOR_MAGENTA] = COLOR(0xf07078),
    [TERMINAL_COLOR_CYAN] = COLOR(0x95e5cb),
    [TERMINAL_COLOR_GREY] = COLOR(0xb3b1ad),

    [TERMINAL_COLOR_BRIGHT_BLACK] = COLOR(0x323232),
    [TERMINAL_COLOR_BRIGHT_RED] = COLOR(0xff6565),
    [TERMINAL_COLOR_BRIGHT_GREEN] = COLOR(0xe9fe83),
    [TERMINAL_COLOR_BRIGHT_YELLOW] = COLOR(0xfff778),
    [TERMINAL_COLOR_BRIGHT_BLUE] = COLOR(0x68d4ff),
    [TERMINAL_COLOR_BRIGHT_MAGENTA] = COLOR(0xffa3aa),
    [TERMINAL_COLOR_BRIGHT_CYAN] = COLOR(0xc7fffc),
    [TERMINAL_COLOR_BRIGHT_GREY] = COLOR(0xffffff),

    [TERMINAL_COLOR_DEFAULT_FOREGROUND] = COLOR(0xb3b1ad),
    [TERMINAL_COLOR_DEFAULT_BACKGROUND] = COLOR(0x0A0E14),
};

Rectangle framebuffer_terminal_cell_bound(int x, int y)
{
    Rectangle bound;
    bound.position = (Point){x * char_size.X, y * (int)(char_size.Y)};
    bound.size = (Point){char_size.X, (char_size.Y)};

    return bound;
}

void framebuffer_terminal_render_cell(Painter *painter, Font *font, int x, int y, TerminalCell cell)
{
    Rectangle bound = framebuffer_terminal_cell_bound(x, y);

    Codepoint codepoint = cell.codepoint;

    Color background_color = framebuffer_colors[cell.attributes.background];
    Color foreground_color = framebuffer_colors[cell.attributes.foreground];
    TerminalAttributes attributes = cell.attributes;

    if (attributes.inverted)
    {
        Color tmp = background_color;
        background_color = foreground_color;
        foreground_color = tmp;
    }

    painter_clear_rectangle(painter, bound, background_color);

    if (attributes.underline)
    {
        painter_draw_line(
            painter,
            point_add(bound.position, (Point){0, 13}),
            point_add(bound.position, (Point){bound.width, 13}),
            foreground_color);
    }

    if (codepoint == U' ')
    {
        return;
    }

    Glyph *glyph = font_glyph(font, cell.codepoint);

    if (glyph != NULL)
    {
        painter_draw_glyph(
            painter,
            font,
            glyph,
            point_add(bound.position, (Point){0, 12}),
            foreground_color);

        if (attributes.bold)
        {
            painter_draw_glyph(
                painter,
                font,
                glyph,
                point_add(bound.position, (Point){1, 12}),
                foreground_color);
        }
    }
    else
    {
        painter_draw_rectangle(painter, bound, foreground_color);
    }
}

void framebuffer_terminal_repaint(Terminal *terminal, FramebufferTerminalRenderer *renderer)
{
    Painter *painter = renderer->framebuffer->painter;

    for (int y = 0; y < terminal->height; y++)
    {
        for (int x = 0; x < terminal->width; x++)
        {
            TerminalCell cell = terminal_cell_at(terminal, x, y);

            if (cell.dirty)
            {
                framebuffer_terminal_render_cell(painter, renderer->mono_font, x, y, cell);

                framebuffer_mark_dirty(renderer->framebuffer, framebuffer_terminal_cell_bound(x, y));

                terminal_cell_undirty(terminal, x, y);
            }
        }
    }

    framebuffer_blit_dirty(renderer->framebuffer);
}

void framebuffer_terminal_render_cursor(Terminal *terminal, FramebufferTerminalRenderer *renderer, int x, int y, bool visible)
{
    Painter *painter = renderer->framebuffer->painter;

    TerminalCell cell = terminal_cell_at(terminal, x, y);

    if (visible)
    {
        cell.attributes.inverted = true;
        cell.attributes.foreground = TERMINAL_COLOR_YELLOW;
    }

    framebuffer_terminal_render_cell(painter, renderer->mono_font, x, y, cell);

    framebuffer_mark_dirty(renderer->framebuffer, framebuffer_terminal_cell_bound(x, y));
}

void framebuffer_terminal_on_cursor(Terminal *terminal, FramebufferTerminalRenderer *renderer, TerminalCursor cursor)
{
    __unused(terminal);

    framebuffer_terminal_render_cursor(terminal, renderer, renderer->framebuffer_cursor.X, renderer->framebuffer_cursor.Y, false);

    renderer->framebuffer_cursor = (Point){cursor.x, cursor.y};

    framebuffer_terminal_render_cursor(terminal, renderer, renderer->framebuffer_cursor.X, renderer->framebuffer_cursor.Y, true);
    renderer->cursor_blink = false;
}

void framebuffer_terminal_on_blink(Terminal *terminal, FramebufferTerminalRenderer *renderer)
{
    renderer->cursor_blink = !renderer->cursor_blink;

    framebuffer_terminal_render_cursor(terminal, renderer, renderer->framebuffer_cursor.X, renderer->framebuffer_cursor.Y, renderer->cursor_blink);

    framebuffer_blit_dirty(renderer->framebuffer);
}

void framebuffer_terminal_destroy(FramebufferTerminalRenderer *renderer)
{
    framebuffer_close(renderer->framebuffer);
    font_destroy(renderer->mono_font);
}

Terminal *framebuffer_terminal_create(void)
{
    Framebuffer *framebuffer = framebuffer_open();

    if (handle_has_error(framebuffer))
    {
        framebuffer_close(framebuffer);

        return NULL;
    }

    painter_clear(framebuffer->painter, framebuffer_colors[TERMINAL_COLOR_DEFAULT_BACKGROUND]);
    framebuffer_blit(framebuffer);

    FramebufferTerminalRenderer *renderer = __create(FramebufferTerminalRenderer);

    TERMINAL_RENDERER(renderer)->on_cursor = (TerminalOnCursorCallback)framebuffer_terminal_on_cursor;
    TERMINAL_RENDERER(renderer)->repaint = (TerminalRepaintCallback)framebuffer_terminal_repaint;
    TERMINAL_RENDERER(renderer)->destroy = (TerminalRendererDestroy)framebuffer_terminal_destroy;
    TERMINAL_RENDERER(renderer)->on_blink = (TerminalOnBlinkCallback)framebuffer_terminal_on_blink;

    renderer->framebuffer = framebuffer;
    renderer->mono_font = font_create("mono");

    return terminal_create(framebuffer->width / char_size.X, framebuffer->height / char_size.Y, TERMINAL_RENDERER(renderer));
}
