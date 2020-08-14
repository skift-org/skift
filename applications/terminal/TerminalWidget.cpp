#include <libgraphic/Painter.h>
#include <libsystem/Logger.h>
#include <libsystem/process/Launchpad.h>
#include <libwidget/Event.h>
#include <libwidget/Window.h>

#include "terminal/Common.h"
#include "terminal/TerminalWidget.h"

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

    terminal_write(widget->terminal(), buffer, size);
    widget->should_repaint();
}

void terminal_widget_cursor_callback(TerminalWidget *widget)
{
    widget->blink();

    int cx = widget->terminal()->cursor.x;
    int cy = widget->terminal()->cursor.y;

    widget->should_repaint(terminal::cell_bound(cx, cy).offset(widget->bound().position()));
}

Terminal *terminal_widget_renderer_create(TerminalWidget *terminal_widget)
{
    TerminalWidgetRenderer *terminal_renderer = __create(TerminalWidgetRenderer);

    terminal_renderer->widget = terminal_widget;

    return terminal_create(80, 24, (TerminalRenderer *)terminal_renderer);
}

TerminalWidget::TerminalWidget(Widget *parent)
    : Widget(parent)
{
    _terminal = terminal_widget_renderer_create(this);

    stream_create_term(
        &_master_stream,
        &_slave_stream);

    _master_notifier = notifier_create(
        this,
        HANDLE(_master_stream),
        SELECT_READ,
        (NotifierCallback)terminal_widget_master_callback);

    _cursor_blink_timer = timer_create(this, 250, (TimerCallback)terminal_widget_cursor_callback);
    timer_start(_cursor_blink_timer);

    Launchpad *shell_launchpad = launchpad_create("shell", "/Applications/shell/shell");
    launchpad_handle(shell_launchpad, HANDLE(_slave_stream), 0);
    launchpad_handle(shell_launchpad, HANDLE(_slave_stream), 1);
    launchpad_handle(shell_launchpad, HANDLE(_slave_stream), 2);
    launchpad_launch(shell_launchpad, nullptr);
}

TerminalWidget::~TerminalWidget()
{
    terminal_destroy(_terminal);

    notifier_destroy(_master_notifier);
    timer_destroy(_cursor_blink_timer);

    stream_close(_master_stream);
    stream_close(_slave_stream);
}

void TerminalWidget::paint(Painter &painter, Rectangle rectangle)
{
    painter.clear_rectangle(rectangle, color(THEME_ANSI_BACKGROUND));

    painter.push();
    painter.transform(bound().position());

    rectangle = rectangle.offset(-bound().position());

    Terminal *terminal = _terminal;

    for (int y = 0; y < terminal->height; y++)
    {
        for (int x = 0; x < terminal->width; x++)
        {
            TerminalCell cell = terminal_cell_at(terminal, x, y);
            terminal::render_cell(painter, x, y, cell);
            terminal_cell_undirty(terminal, x, y);
        }
    }

    int cx = terminal->cursor.x;
    int cy = terminal->cursor.y;

    if (terminal::cell_bound(cx, cy).colide_with(rectangle))
    {
        TerminalCell cell = terminal_cell_at(terminal, cx, cy);

        if (window_is_focused(window()))
        {
            if (_cursor_blink)
            {
                terminal::render_cell(
                    painter,
                    cx,
                    cy,
                    cell.codepoint,
                    TERMINAL_COLOR_DEFAULT_BACKGROUND,
                    TERMINAL_COLOR_DEFAULT_FOREGROUND,
                    _terminal->default_attributes);
            }
            else
            {
                terminal::render_cell(
                    painter,
                    cx,
                    cy,
                    cell);
            }
        }
        else
        {
            terminal::render_cell(painter, cx, cy, cell);
            painter.draw_rectangle(terminal::cell_bound(cx, cy), color(THEME_ANSI_CURSOR));
        }
    }

    painter.pop();
}

void TerminalWidget::event(Event *event)
{
    if (event->type == Event::KEYBOARD_KEY_TYPED)
    {
        if (event->keyboard.key == KEYBOARD_KEY_UP)
        {
            stream_format(_master_stream, "\e[A");
            event->accepted = true;
        }
        else if (event->keyboard.key == KEYBOARD_KEY_DOWN)
        {
            stream_format(_master_stream, "\e[B");
            event->accepted = true;
        }
        else if (event->keyboard.key == KEYBOARD_KEY_RIGHT)
        {
            stream_format(_master_stream, "\e[C");
            event->accepted = true;
        }
        else if (event->keyboard.key == KEYBOARD_KEY_LEFT)
        {
            stream_format(_master_stream, "\e[D");
            event->accepted = true;
        }
        else if (event->keyboard.codepoint != 0)
        {
            uint8_t buffer[5];
            int size = codepoint_to_utf8(event->keyboard.codepoint, buffer);
            stream_write(_master_stream, buffer, size);
            event->accepted = true;
        }
    }
}

void TerminalWidget::do_layout()
{
    int width = bound().width() / terminal::cell_size().x();
    int height = bound().height() / terminal::cell_size().y();

    width = MAX(width, 8);
    height = MAX(height, 8);

    if (_terminal->width != width ||
        _terminal->height != height)
    {
        terminal_resize(_terminal, width, height);
    }
}
