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
        handle_printf_error(master, "Terminal: read from master failed");
        return;
    }

    terminal_write(widget->terminal(), buffer, size);
    widget->should_repaint();
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

    _cursor_blink_timer = make<Timer>(250, [this]() {
        blink();

        int cx = terminal()->cursor.x;
        int cy = terminal()->cursor.y;

        should_repaint(terminal::cell_bound(cx, cy).offset(bound().position()));
    });

    _cursor_blink_timer->start();

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
    auto send_sequence = [&](auto sequence) {
        stream_format(_master_stream, sequence);
        event->accepted = true;
    };

    if (event->type == Event::KEYBOARD_KEY_TYPED)
    {
        switch (event->keyboard.key)
        {

        case KEYBOARD_KEY_DELETE:
            send_sequence("\e[3~");
            break;
        case KEYBOARD_KEY_END:
            send_sequence("\e[4~");
            break;
        case KEYBOARD_KEY_F1:
            send_sequence("\e[11~");
            break;
        case KEYBOARD_KEY_F2:
            send_sequence("\e[12~");
            break;
        case KEYBOARD_KEY_F3:
            send_sequence("\e[13~");
            break;
        case KEYBOARD_KEY_F4:
            send_sequence("\e[14~");
            break;
        case KEYBOARD_KEY_F5:
            send_sequence("\e[15~");
            break;
        case KEYBOARD_KEY_F6:
            send_sequence("\e[17~");
            break;
        case KEYBOARD_KEY_F7:
            send_sequence("\e[18~");
            break;
        case KEYBOARD_KEY_F8:
            send_sequence("\e[19~");
            break;
        case KEYBOARD_KEY_F9:
            send_sequence("\e[20~");
            break;
        case KEYBOARD_KEY_F10:
            send_sequence("\e[21~");
            break;
        case KEYBOARD_KEY_F11:
            send_sequence("\e[23~");
            break;
        case KEYBOARD_KEY_F12:
            send_sequence("\e[24~");
            break;
        case KEYBOARD_KEY_HOME:
            send_sequence("\e[1~");
            break;
        case KEYBOARD_KEY_INSERT:
            send_sequence("\e[2~");
            break;
        case KEYBOARD_KEY_PGUP:
            send_sequence("\e[5~");
            break;
        case KEYBOARD_KEY_PGDOWN:
            send_sequence("\e[6~");
            break;

        case KEYBOARD_KEY_UP:
            send_sequence("\e[A");
            break;
        case KEYBOARD_KEY_DOWN:
            send_sequence("\e[B");
            break;
        case KEYBOARD_KEY_RIGHT:
            send_sequence("\e[C");
            break;
        case KEYBOARD_KEY_LEFT:
            send_sequence("\e[D");
            break;

        default:
            if (event->keyboard.codepoint != 0)
            {
                uint8_t buffer[5];
                int size = codepoint_to_utf8(event->keyboard.codepoint, buffer);
                stream_write(_master_stream, buffer, size);
                event->accepted = true;
            }
            break;
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
