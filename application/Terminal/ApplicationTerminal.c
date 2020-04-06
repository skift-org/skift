#include <libgraphic/Painter.h>
#include <libsystem/logger.h>
#include <libsystem/process/Launchpad.h>
#include <libwidget/Event.h>
#include <libwidget/Theme.h>

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

    TerminalCell cell = terminal_cell_at(terminal, terminal->cursor.x, terminal->cursor.y);

    int cx = terminal->cursor.x;
    int cy = terminal->cursor.y;

    if (terminal_widget->cursor_blink)
    {
        cell.attributes.inverted = true;
        cell.attributes.foreground = TERMINAL_COLOR_YELLOW;
    }

    terminal_widget->cursor_blink = !terminal_widget->cursor_blink;

    terminal_widget_render_cell(painter, get_terminal_font(), cx, cy, cell);

    painter_pop_origin(painter);
}

#define TERMINAL_IO_BUFFER_SIZE 4096

void terminal_widget_master_callback(TerminalWidget *widget, Stream *master, SelectEvent events)
{
    __unused(events);

    char buffer[TERMINAL_IO_BUFFER_SIZE];
    size_t size = stream_read(master, buffer, TERMINAL_IO_BUFFER_SIZE);

    if (handle_has_error(master))
    {
        handle_printf_error(master, "Terminal: read from master failled");
        return;
    }

    terminal_write(widget->terminal, buffer, size);
    widget_update(WIDGET(widget));
}

void terminal_widget_cursor_callback(TerminalWidget *widget)
{
    // FIXME: don't update the whole widget juste to repaint the cursor.
    widget_update(WIDGET(widget));
}

void terminal_widget_renderer_create(TerminalWidget *terminal_widget)
{
    TerminalWidgetRenderer *terminal_renderer = __create(TerminalWidgetRenderer);

    terminal_renderer->widget = terminal_widget;

    Terminal *terminal = terminal_create(80, 24, (TerminalRenderer *)terminal_renderer);

    terminal_widget->terminal = terminal;
}

void terminal_widget_event_callback(TerminalWidget *terminal_widget, Event *event)
{
    if (event->type == EVENT_KEYBOARD_KEY_TYPED)
    {
        KeyboardEvent *key_event = (KeyboardEvent *)event;
        stream_write(terminal_widget->master_stream, &key_event->codepoint, sizeof(char));

        event->accepted = true;
    }
}

void terminal_widget_destroy(TerminalWidget *terminal_widget)
{
    terminal_destroy(terminal_widget->terminal);

    notifier_destroy(terminal_widget->master_notifier);
    timer_destroy(terminal_widget->cursor_blink_timer);

    stream_close(terminal_widget->master_stream);
    stream_close(terminal_widget->slave_stream);
}

Widget *terminal_widget_create(Widget *parent)
{
    TerminalWidget *widget = __create(TerminalWidget);

    widget_initialize(WIDGET(widget), "Terminal", parent, (Rectangle){});

    WIDGET(widget)->paint = (WidgetPaintCallback)terminal_widget_paint;
    WIDGET(widget)->event = (WidgetEventCallback)terminal_widget_event_callback;

    terminal_widget_renderer_create(widget);

    stream_create_term(
        &widget->master_stream,
        &widget->slave_stream);

    widget->master_notifier = notifier_create(
        widget,
        HANDLE(widget->master_stream),
        SELECT_READ,
        (NotifierCallback)terminal_widget_master_callback);

    widget->cursor_blink_timer = timer_create(widget, 250, (TimerCallback)terminal_widget_cursor_callback);
    timer_start(widget->cursor_blink_timer);

    Launchpad *shell_launchpad = launchpad_create("Shell", "/bin/Shell");
    launchpad_handle(shell_launchpad, HANDLE(widget->slave_stream), 0);
    launchpad_handle(shell_launchpad, HANDLE(widget->slave_stream), 1);
    launchpad_handle(shell_launchpad, HANDLE(widget->slave_stream), 2);
    launchpad_launch(shell_launchpad, NULL);

    return WIDGET(widget);
}
