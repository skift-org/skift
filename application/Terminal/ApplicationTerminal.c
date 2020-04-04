#include <libgraphic/Painter.h>
#include <libsystem/logger.h>
#include <libwidget/core/Theme.h>

#include "Terminal/ApplicationTerminal.h"

static Point _cell_size = (Point){7, 16};

static Color _colors[__TERMINAL_COLOR_COUNT] = {
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

Font *get_terminal_font(void)
{
    static Font *font = NULL;

    if (font == NULL)
    {
        font = font_create("mono");
    }

    return font;
}

Rectangle terminal_widget_cell_bound(int x, int y)
{
    Rectangle bound = {};

    bound.position = (Point){x * _cell_size.X, y * (int)(_cell_size.Y)};
    bound.size = (Point){_cell_size.X, (_cell_size.Y)};

    return bound;
}

void terminal_widget_render_cell(Painter *painter, Font *font, int x, int y, TerminalCell cell)
{
    Rectangle bound = terminal_widget_cell_bound(x, y);

    Codepoint codepoint = cell.codepoint;

    Color background_color = _colors[cell.attributes.background];
    Color foreground_color = _colors[cell.attributes.foreground];
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

void terminal_widget_paint(TerminalWidget *terminal_widget, Painter *painter)
{
    logger_trace("Terminal repaint!");

    painter_push_origin(painter, widget_bound(terminal_widget).position);

    Terminal *terminal = terminal_widget->terminal;

    for (int y = 0; y < terminal->height; y++)
    {
        for (int x = 0; x < terminal->width; x++)
        {
            TerminalCell cell = terminal_cell_at(terminal, x, y);

            terminal_widget_render_cell(painter, get_terminal_font(), x, y, cell);
            terminal_cell_undirty(terminal, x, y);
        }
    }

    painter_pop_origin(painter);
}

void terminal_widget_renderer_create(TerminalWidget *terminal_widget)
{
    TerminalWidgetRenderer *terminal_renderer = __create(TerminalWidgetRenderer);

    terminal_renderer->widget = terminal_widget;

    Terminal *terminal = terminal_create(80, 24, (TerminalRenderer *)terminal_renderer);

    terminal_widget->terminal = terminal;
}

void terminal_widget_destroy(TerminalWidget *terminal_widget)
{
    terminal_destroy(terminal_widget->terminal);
}

Widget *terminal_widget_create(Widget *parent)
{
    TerminalWidget *terminal_widget = __create(TerminalWidget);

    widget_initialize(WIDGET(terminal_widget), "Terminal", parent, (Rectangle){});

    WIDGET(terminal_widget)->paint = (WidgetPaintCallback)terminal_widget_paint;

    terminal_widget_renderer_create(terminal_widget);

    terminal_write(terminal_widget->terminal, "Hello, world!\n", 14);

    return WIDGET(terminal_widget);
}
