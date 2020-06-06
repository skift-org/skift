#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libsystem/process/Launchpad.h>
#include <libwidget/Event.h>
#include <libwidget/Window.h>

#include "terminal/ApplicationTerminal.h"

static Vec2i _cell_size = vec2i(7, 16);

static ThemeColorRole terminal_color_to_role[__TERMINAL_COLOR_COUNT] = {
    [TERMINAL_COLOR_DEFAULT_FOREGROUND] = THEME_ANSI_FOREGROUND,
    [TERMINAL_COLOR_DEFAULT_BACKGROUND] = THEME_ANSI_BACKGROUND,
    [TERMINAL_COLOR_BLACK] = THEME_ANSI_BLACK,
    [TERMINAL_COLOR_RED] = THEME_ANSI_RED,
    [TERMINAL_COLOR_GREEN] = THEME_ANSI_GREEN,
    [TERMINAL_COLOR_YELLOW] = THEME_ANSI_YELLOW,
    [TERMINAL_COLOR_BLUE] = THEME_ANSI_BLUE,
    [TERMINAL_COLOR_MAGENTA] = THEME_ANSI_MAGENTA,
    [TERMINAL_COLOR_CYAN] = THEME_ANSI_CYAN,
    [TERMINAL_COLOR_GREY] = THEME_ANSI_WHITE,
    [TERMINAL_COLOR_BRIGHT_BLACK] = THEME_ANSI_BRIGHT_BLACK,
    [TERMINAL_COLOR_BRIGHT_RED] = THEME_ANSI_BRIGHT_RED,
    [TERMINAL_COLOR_BRIGHT_GREEN] = THEME_ANSI_BRIGHT_GREEN,
    [TERMINAL_COLOR_BRIGHT_YELLOW] = THEME_ANSI_BRIGHT_YELLOW,
    [TERMINAL_COLOR_BRIGHT_BLUE] = THEME_ANSI_BRIGHT_BLUE,
    [TERMINAL_COLOR_BRIGHT_MAGENTA] = THEME_ANSI_BRIGHT_MAGENTA,
    [TERMINAL_COLOR_BRIGHT_CYAN] = THEME_ANSI_BRIGHT_CYAN,
    [TERMINAL_COLOR_BRIGHT_GREY] = THEME_ANSI_BRIGHT_WHITE,
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

Rectangle terminal_widget_cell_bound(TerminalWidget *widget, int x, int y)
{
    Rectangle bound = {};

    bound.position = vec2i(widget_get_bound(widget).x + x * _cell_size.x, widget_get_bound(widget).y + y * (int)(_cell_size.y));
    bound.size = vec2i(_cell_size.x, (_cell_size.y));

    return bound;
}

void terminal_widget_render_cell_extended(TerminalWidget *widget, Painter *painter, int x, int y, Codepoint codepoint, Color foreground, Color background, TerminalAttributes attributes)
{
    Rectangle bound = terminal_widget_cell_bound(widget, x, y);

    if (attributes.inverted)
    {
        Color tmp = background;
        background = foreground;
        foreground = tmp;
    }

    painter_clear_rectangle(painter, bound, background);

    if (attributes.underline)
    {
        painter_draw_line(
            painter,
            vec2i_add(bound.position, vec2i(0, 13)),
            vec2i_add(bound.position, vec2i(bound.width, 13)),
            foreground);
    }

    if (codepoint == U' ')
    {
        return;
    }

    Glyph *glyph = font_glyph(get_terminal_font(), codepoint);

    if (glyph != NULL)
    {
        painter_draw_glyph(
            painter,
            get_terminal_font(),
            glyph,
            vec2i_add(bound.position, vec2i(0, 12)),
            foreground);

        if (attributes.bold)
        {
            painter_draw_glyph(
                painter,
                get_terminal_font(),
                glyph,
                vec2i_add(bound.position, vec2i(1, 12)),
                foreground);
        }
    }
    else
    {
        painter_draw_rectangle(painter, bound, foreground);
    }
}

void terminal_widget_render_cell(TerminalWidget *widget, Painter *painter, int x, int y, TerminalCell cell)
{
    Color background_color = widget_get_color(WIDGET(widget), terminal_color_to_role[cell.attributes.background]);
    Color foreground_color = widget_get_color(WIDGET(widget), terminal_color_to_role[cell.attributes.foreground]);

    terminal_widget_render_cell_extended(widget, painter, x, y, cell.codepoint, foreground_color, background_color, cell.attributes);
}

void terminal_widget_paint(TerminalWidget *terminal_widget, Painter *painter, Rectangle rectangle)
{
    __unused(rectangle);

    Terminal *terminal = terminal_widget->terminal;

    for (int y = 0; y < terminal->height; y++)
    {
        for (int x = 0; x < terminal->width; x++)
        {
            TerminalCell cell = terminal_cell_at(terminal, x, y);

            terminal_widget_render_cell(terminal_widget, painter, x, y, cell);
            terminal_cell_undirty(terminal, x, y);
        }
    }

    int cx = terminal->cursor.x;
    int cy = terminal->cursor.y;
    if (rectangle_colide(terminal_widget_cell_bound(terminal_widget, cx, cy), rectangle))
    {
        TerminalCell cell = terminal_cell_at(terminal, cx, cy);

        if (window_is_focused(WIDGET(terminal_widget)->window))
        {
            if (terminal_widget->cursor_blink)
            {
                terminal_widget_render_cell_extended(
                    terminal_widget,
                    painter,
                    cx,
                    cy,
                    cell.codepoint,
                    widget_get_color(WIDGET(terminal_widget), THEME_ANSI_FOREGROUND),
                    widget_get_color(WIDGET(terminal_widget), THEME_ANSI_CURSOR), cell.attributes);
            }
            else
            {
                terminal_widget_render_cell(terminal_widget, painter, cx, cy, cell);
            }
        }
        else
        {
            terminal_widget_render_cell(terminal_widget, painter, cx, cy, cell);
            painter_draw_rectangle(painter, terminal_widget_cell_bound(terminal_widget, cx, cy), widget_get_color(WIDGET(terminal_widget), THEME_ANSI_CURSOR));
        }
    }
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
    widget->cursor_blink = !widget->cursor_blink;

    int cx = widget->terminal->cursor.x;
    int cy = widget->terminal->cursor.y;
    widget_update_region(WIDGET(widget), terminal_widget_cell_bound(widget, cx, cy));
}

void terminal_widget_renderer_create(TerminalWidget *terminal_widget)
{
    TerminalWidgetRenderer *terminal_renderer = __create(TerminalWidgetRenderer);

    terminal_renderer->widget = terminal_widget;

    Terminal *terminal = terminal_create(80, 24, (TerminalRenderer *)terminal_renderer);

    terminal_widget->terminal = terminal;
}

void terminal_widget_event(TerminalWidget *terminal_widget, Event *event)
{
    if (event->type == EVENT_KEYBOARD_KEY_TYPED)
    {
        if (event->keyboard.key == KEYBOARD_KEY_UP)
        {
            stream_printf(terminal_widget->master_stream, "\e[A");
            event->accepted = true;
        }
        else if (event->keyboard.key == KEYBOARD_KEY_UP)
        {
            stream_printf(terminal_widget->master_stream, "\e[B");
            event->accepted = true;
        }
        else if (event->keyboard.key == KEYBOARD_KEY_UP)
        {
            stream_printf(terminal_widget->master_stream, "\e[C");
            event->accepted = true;
        }
        else if (event->keyboard.key == KEYBOARD_KEY_UP)
        {
            stream_printf(terminal_widget->master_stream, "\e[D");
            event->accepted = true;
        }
        else if (event->keyboard.codepoint != 0)
        {
            uint8_t buffer[5];
            int size = codepoint_to_utf8(event->keyboard.codepoint, buffer);
            stream_write(terminal_widget->master_stream, buffer, size);
            event->accepted = true;
        }
    }
}

void terminal_widget_layout(TerminalWidget *widget)
{
    int width = widget_get_bound(widget).width / _cell_size.width;
    int height = widget_get_bound(widget).height / _cell_size.height;

    width = MAX(width, 8);
    height = MAX(height, 8);

    if (widget->terminal->width != width ||
        widget->terminal->height != height)
    {
        terminal_resize(widget->terminal, width, height);
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

static const WidgetClass terminal_class = {
    .name = "Terminal",

    .paint = (WidgetPaintCallback)terminal_widget_paint,
    .event = (WidgetEventCallback)terminal_widget_event,
    .layout = (WidgetLayoutCallback)terminal_widget_layout,
};

Widget *terminal_widget_create(Widget *parent)
{
    TerminalWidget *widget = __create(TerminalWidget);

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

    Launchpad *shell_launchpad = launchpad_create("shell", "/bin/shell");
    launchpad_handle(shell_launchpad, HANDLE(widget->slave_stream), 0);
    launchpad_handle(shell_launchpad, HANDLE(widget->slave_stream), 1);
    launchpad_handle(shell_launchpad, HANDLE(widget->slave_stream), 2);
    launchpad_launch(shell_launchpad, NULL);

    widget_initialize(WIDGET(widget), &terminal_class, parent);

    return WIDGET(widget);
}
